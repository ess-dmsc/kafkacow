@Library('ecdc-pipeline')
import ecdcpipeline.ContainerBuildNode
import ecdcpipeline.PipelineBuilder

project = "kafkacow"

clangformat_os = "debian9"
test_os = "centos7"
archive_os = "centos7-release"

container_build_nodes = [
  'centos7': ContainerBuildNode.getDefaultContainerBuildNode('centos7'),
  'centos7-release': ContainerBuildNode.getDefaultContainerBuildNode('centos7'),
  'debian9': ContainerBuildNode.getDefaultContainerBuildNode('debian9'),
  'ubuntu1804': ContainerBuildNode.getDefaultContainerBuildNode('ubuntu1804')
]

def num_artifacts_to_keep
if (env.BRANCH_NAME == 'master') {
  num_artifacts_to_keep = '5'
} else {
  num_artifacts_to_keep = '1'
}

// Set number of old builds to keep.
properties([[
  $class: 'BuildDiscarderProperty',
  strategy: [
    $class: 'LogRotator',
    artifactDaysToKeepStr: '',
    artifactNumToKeepStr: num_artifacts_to_keep,
    daysToKeepStr: '',
    numToKeepStr: num_artifacts_to_keep
  ]
]]);

pipeline_builder = new PipelineBuilder(this, container_build_nodes)
pipeline_builder.activateEmailFailureNotifications()

builders = pipeline_builder.createBuilders { container ->
  pipeline_builder.stage("${container.key}: checkout") {
    dir(pipeline_builder.project) {
      checkout scm
      sh "git submodule update --init --recursive"
    }
    // Copy source code to container
    container.copyTo(pipeline_builder.project, pipeline_builder.project)
  }  // stage

  pipeline_builder.stage("${container.key}: get dependencies") {
    container.sh """
      mkdir build
      cd build
      conan remote add --insert 0 ess-dmsc-local ${local_conan_server}
      conan install --build outdated ../${pipeline_builder.project}
    """
  }  // stage

  pipeline_builder.stage("${container.key}: configure") {
    def extra_cmake_options = ""
    if (container.key == test_os) {
      extra_cmake_options = "-DCOV=1"
    } else if (container.key == archive_os) {
      extra_cmake_options = "-DCMAKE_BUILD_TYPE=Release -DCMAKE_SKIP_RPATH=FALSE -DCMAKE_INSTALL_RPATH='\\\\\\\$ORIGIN/../lib' -DCMAKE_BUILD_WITH_INSTALL_RPATH=TRUE"
    }
    container.sh """
      cd build
      . ./activate_run.sh
      cmake ../${pipeline_builder.project} ${extra_cmake_options}
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
        lcov --directory . --capture --output-file coverage.info
        lcov --remove coverage.info '*_generated.h' '*/.conan/data/*' '*/usr/*' '*Test.cpp' '*gmock*' '*gtest*' --output-file coverage.info
      """
      container.copyFrom('build', '.')
      junit "build/${test_output}"
      
      withCredentials([string(credentialsId: 'kafkacow-codecov-token', variable: 'TOKEN')]) {
        sh "cp ${project}/codecov.yml codecov.yml"
        sh "curl -s https://codecov.io/bash | bash -s - -f build/coverage.info -t ${TOKEN} -C ${scm_vars.GIT_COMMIT}"
      }
    }  // stage
  }  // if

  if (container.key == clangformat_os) {
    pipeline_builder.stage("${container.key}: check formatting") {
      container.sh """
        clang-format -version
        cd ${pipeline_builder.project}
        find . \\\\( -name '*.cpp' -or -name '*.cxx' -or -name '*.h' -or -name '*.hpp' \\\\) \\
          -exec clangformatdiff.sh {} +
      """
    }  // stage

    pipeline_builder.stage("${container.key}: cppcheck") {
      def test_output = "cppcheck.txt"
      container.sh """
        cd ${pipeline_builder.project}
        cppcheck --enable=all --inconclusive --template="{file},{line},{severity},{id},{message}" src/ 2> ${test_output}
      """
      container.copyFrom("${pipeline_builder.project}/${test_output}", '.')
      step([
        $class: 'WarningsPublisher',
        parserConfigurations: [[
          parserName: 'Cppcheck Parser',
          pattern: 'cppcheck.txt'
        ]]
      ])
    }  // stage
  }  // if
  
    if (container.key == archive_os) {
    pipeline_builder.stage("${container.key}: Archive build") {
      def archive_output = "${pipeline_builder.project}-${container.key}.tar.gz"
      container.sh """
        cd build
        rm -rf ${pipeline_builder.project}; mkdir ${pipeline_builder.project}
        mkdir ${pipeline_builder.project}/bin
        cp ./bin/${pipeline_builder.project} ${pipeline_builder.project}/bin/
        cp -r ./lib ${pipeline_builder.project}/
        cp -r ./licenses ${pipeline_builder.project}/
        cp -r ./schemas ${pipeline_builder.project}/
        tar czf ${archive_output} ${pipeline_builder.project}
        # Create file with build information
        touch BUILD_INFO
        echo 'Repository: ${pipeline_builder.project}/${env.BRANCH_NAME}' >> BUILD_INFO
        echo 'Commit: ${scm_vars.GIT_COMMIT}' >> BUILD_INFO
        echo 'Jenkins build: ${env.BUILD_NUMBER}' >> BUILD_INFO
      """

      container.copyFrom("build/${archive_output}", '.')
      container.copyFrom('build/BUILD_INFO', '.')
      archiveArtifacts "${archive_output},BUILD_INFO"
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
        sh "git submodule update --init --recursive"
      } catch (e) {
        failure_function(e, 'Checkout failed')
      }
    }
  }

  builders['macOS'] = get_macos_pipeline()
  builders['windows10'] = get_win10_pipeline()
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
                        checkout scm
                        sh "git submodule update --init --recursive"
                    } catch (e) {
                        failure_function(e, 'MacOSX / Checkout failed')
                    }
                }

                dir("${project}/build") {
                    try {
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

def get_win10_pipeline() {
    return {
        node('windows10') {

        // Use custom location to avoid Win32 path length issues
            ws('c:\\jenkins\\') {
                cleanWs()
                dir("${project}") {
                    stage("win10: Checkout") {
                      checkout scm
                      bat "git submodule update --init --recursive"
                    }  // stage

                    stage("win10: Setup") {
                      // Conan remove is temporary until all projects have moved to lowercase package name
                      bat """conan remove -f FlatBuffers/*
                        if exist _build rd /q /s _build
                        mkdir _build
                        """
                    } // stage

                    stage("win10: Install") {
                      bat """cd _build
                    conan.exe \
                        install ..\\conanfile.txt  \
                        --settings build_type=Release \
                        --build=outdated"""
                    }  // stage

                    stage("win10: Build") {
                           bat """cd _build
                        cmake .. -G \"Visual Studio 15 2017 Win64\" -DCMAKE_BUILD_TYPE=Release -DCONAN=MANUAL -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE
                        cmake --build . --config Release
                        """
                    }  // stage
                    
                    stage("win10: Test") {
                           bat """cd _build
                        activate_run.bat && .\\bin\\UnitTests.exe && deactivate_run.bat
                        """
                    }  // stage
                }  // dir
            }  // ws
        }  // node
    }  // return
} // def
