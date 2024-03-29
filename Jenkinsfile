@Library('ecdc-pipeline')
import ecdcpipeline.ContainerBuildNode
import ecdcpipeline.PipelineBuilder

project = "kafkacow"

clangformat_os = "ubuntu2204"
test_os = "centos7"
release_os = "centos7-release"

container_build_nodes = [
    'centos7': ContainerBuildNode.getDefaultContainerBuildNode('centos7-gcc11'),
    'centos7-release': ContainerBuildNode.getDefaultContainerBuildNode('centos7-gcc11'),
    'ubuntu2204' : ContainerBuildNode.getDefaultContainerBuildNode('ubuntu2204')
]

pipeline_builder = new PipelineBuilder(this, container_build_nodes)
pipeline_builder.activateEmailFailureNotifications()

builders = pipeline_builder.createBuilders { container ->
  pipeline_builder.stage("${container.key}: checkout") {
    dir(pipeline_builder.project) {
      checkout scm
    }
    // Copy source code to container
    container.copyTo(pipeline_builder.project, pipeline_builder.project)
  }  // stage

  // Use static libraries for archived artefact
  if (container.key == release_os) {
    container.sh """
      cd ${pipeline_builder.project}
      sed -i 's/shared=True/shared=False/' conanfile.txt
      cat conanfile.txt
    """
  }

  pipeline_builder.stage("${container.key}: get dependencies") {
    container.sh """
      mkdir build
      cd build
      conan install --build outdated ../${pipeline_builder.project}
    """
  }  // stage

  pipeline_builder.stage("${container.key}: configure") {
    def coverage_on = ""
    if (container.key == test_os) {
      coverage_on = "-DCOV=1"
    }
    container.sh """
      cd build
      . ./activate_run.sh
      cmake ../${pipeline_builder.project} ${coverage_on}
    """
  }  // stage

  pipeline_builder.stage("${container.key}: build") {
    container.sh """
      cd build
      . ./activate_run.sh
      make VERBOSE=1 all > ${container.key}-build.log
    """
    container.copyFrom("build/${container.key}-build.log", "${container.key}-build.log")
    archiveArtifacts "${container.key}-build.log"
  }  // stage

  if (container.key == test_os) {
    pipeline_builder.stage("${container.key}: test") {
      def test_output = "TestResults.xml"
      container.sh """
        cd build
        . ./activate_run.sh
        ./bin/UnitTests --gtest_output=xml:${test_output}
        make coverage
      """
      container.copyFrom('build', '.')
      junit "build/${test_output}"
      
      step([
          $class: 'CoberturaPublisher',
          autoUpdateHealth: true,
          autoUpdateStability: true,
          coberturaReportFile: 'build/coverage.xml',
          failUnhealthy: false,
          failUnstable: false,
          maxNumberOfBuilds: 0,
          onlyStable: false,
          sourceEncoding: 'ASCII',
          zoomCoverageChart: true
      ])
    }  // stage
  }  // if

  if (container.key == release_os) {
    pipeline_builder.stage("${container.key}: Archiving") {
      def archive_output = "${pipeline_builder.project}-${container.key}.tar.gz"
      container.sh """
        mkdir archive
        cd archive
        mkdir -p ${pipeline_builder.project}/bin
        cp ../build/bin/kafkacow ${pipeline_builder.project}/bin/
        cp -r ../build/licenses ${pipeline_builder.project}/
        cp -r ../build/schemas ${pipeline_builder.project}/

        # Create file with build information
        cd ${pipeline_builder.project}
        touch BUILD_INFO
        echo 'Repository: ${pipeline_builder.project}/${env.BRANCH_NAME}' >> BUILD_INFO
        echo 'Commit: ${scm_vars.GIT_COMMIT}' >> BUILD_INFO
        echo 'Jenkins build: ${env.BUILD_NUMBER}' >> BUILD_INFO
        cd ..

        tar czf ${archive_output} ${pipeline_builder.project}
      """

      container.copyFrom("archive/${archive_output}", '.')
      archiveArtifacts "${archive_output}"
    }  // stage
  }

  if (container.key == clangformat_os && env.CHANGE_ID) {
    pipeline_builder.stage("${container.key}: check formatting") {
    try {
      container.setupLocalGitUser(pipeline_builder.project)
      container.sh """
          clang-format -version
          cd ${pipeline_builder.project}
          find . \\\\( -name '*.cpp' -or -name '*.cxx' -or -name '*.h' -or -name '*.hpp' \\\\) \\
            -exec clang-format -i {} +
          git status -s
          git add -u
          git commit -m 'GO FORMAT YOURSELF (clang-format)'
        """
      } catch (e) {
        // Okay to fail as there could be no badly formatted files to commit
      } finally {
        // Clean up
      }

      // Push any changes resulting from formatting
      container.copyFrom(pipeline_builder.project, 'clang-formatted-code')
      try {
        withCredentials([gitUsernamePassword(
          credentialsId: 'cow-bot-username-with-token',
          gitToolName: 'Default'
        )]) {
          withEnv(["PROJECT=${pipeline_builder.project}"]) {
            sh '''
              cd clang-formatted-code
              git push https://github.com/ess-dmsc/kafkacow.git HEAD:$CHANGE_BRANCH
            '''
          }  // withEnv
        }  // withCredentials
      } catch (e) {
        // Okay to fail; there may be nothing to push
      } finally {
        // Clean up
      }
    }  // stage

    pipeline_builder.stage("${container.key}: cppcheck") {
      def test_output = "cppcheck.xml"
      container.sh """
        cd ${pipeline_builder.project}
        cppcheck --enable=all --inconclusive --template="{file},{line},{severity},{id},{message}" --xml --xml-version=2 src/ 2> ${test_output}
      """
      container.copyFrom("${pipeline_builder.project}/${test_output}", '.')
      recordIssues(tools: [cppCheck(pattern: 'cppcheck.xml')])
    }  // stage
  }  // if
}  // createBuilders

node {
  // Delete workspace when build is done
  cleanWs()

  stage('Checkout') {
    dir("${project}") {
      try {
        scm_vars = checkout scm
      } catch (e) {
        failure_function(e, 'Checkout failed')
      }
    }
  }

  if (env.ENABLE_MACOS_BUILDS.toUpperCase() == 'TRUE') {
    builders['macOS'] = get_macos_pipeline()
  }

  try {
    parallel builders
    } catch (e) {
      pipeline_builder.handleFailureMessages()
      throw e
  }
}

def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage + '\"\n\nCheck console output at $BUILD_URL to view the results.', recipientProviders: toEmails, subject: '${DEFAULT_SUBJECT}'
    throw exception_obj
}

def get_macos_pipeline()
{
    return {
        stage("macOS") {
            node ("macos") {
                // Delete workspace when build is done
                cleanWs()

                dir("${project}/code") {
                    try {
                        // Conan remove is temporary until all projects have moved to lowercase package name
                        sh "conan remove -f FlatBuffers/*"
                        sh "conan remove -f OpenSSL/*"
                        checkout scm
                    } catch (e) {
                        failure_function(e, 'MacOSX / Checkout failed')
                    }
                }

                dir("${project}/build") {
                    try {
                        // Workaround for issue due to case-sensitivity in package names.
                        sh "conan remove --force 'cli11*'"
                        sh "conan install --build=outdated ../code"
                        sh "source activate_run.sh && cmake ../code"
                    } catch (e) {
                        failure_function(e, 'MacOSX / CMake failed')
                    }

                    try {
                        sh "source activate_run.sh && make all"
                        sh "source activate_run.sh && ./bin/UnitTests --gtest_output=xml:TestResults.xml"
                    } catch (e) {
                        failure_function(e, 'MacOSX / build+test failed')
                    }
                }

            }
        }
    }
}
