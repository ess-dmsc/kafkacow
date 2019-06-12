#include "UpdateSchemas.h"
#include <boost/dll.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <git2.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

namespace {
git_repository *tryRepositoryClone(std::shared_ptr<spdlog::logger> &Logger,
                                   const std::string &RepoURL,
                                   const std::string &RepoPath) {
  boost::filesystem::path Repo(RepoPath);
  git_repository *repo = nullptr;
  int Error = 0;
  if (!exists(Repo)) {
    Error = git_clone(&repo, RepoURL.c_str(), RepoPath.c_str(), nullptr);
    if (Error != 0) {
      Logger->error("Error cloning schema repository.");
    }
  } else {
    Error = git_repository_open(&repo, RepoPath.c_str());
    if (Error != 0) {
      Logger->error("Error opening schema repository.");
    }
  }
  return repo;
}

void cleanupGit(git_repository *repo) {
  git_repository_free(repo);
  git_libgit2_shutdown();
}

int tryRepositoryPull(git_repository *Repo) {
  git_remote *Remote = nullptr;
  git_remote_lookup(&Remote, Repo, "origin");
  const std::string FetchCmd = "pull";
  int Error = git_remote_fetch(
      Remote, nullptr, /* refspecs, nullptr to use the configured ones */
      nullptr,         /* options, nullptr for defaults */
      FetchCmd.c_str());
  git_remote_free(Remote);
  return Error;
}
}

/// Update schemas from github repository at runtime if possible, else fall back
/// on using schema included at build time
/// \return Path to schema directory
std::string updateSchemas(bool PullUpdates) {
  boost::system::error_code Error;
  auto BinDirPath = boost::dll::program_location(Error).parent_path();
  auto BuildDir = BinDirPath.parent_path();
  auto SchemaPath = BuildDir / "schemas";

  if (!PullUpdates) {
    return SchemaPath.string();
  }

  git_libgit2_init();
  std::shared_ptr<spdlog::logger> Logger = spdlog::get("LOG");

  const std::string RepoURL = "git://github.com/ess-dmsc/streaming-data-types";
  auto RepoPath = BuildDir / "streaming-data-types";

  Logger->debug("Updating schema information...");

  git_repository *Repo = tryRepositoryClone(Logger, RepoURL, RepoPath.string());
  if (Repo == nullptr) {
    Logger->warn("Could not get up-to-date schema information, falling back on "
                 "possibly-outdated schemas.");
    return SchemaPath.string();
  }

  int GitError = tryRepositoryPull(Repo);

  cleanupGit(Repo);

  if (GitError != 0) {
    Logger->warn("Could not get up-to-date schema information, falling back on "
                 "possibly-outdated schemas.");
  } else {
    Logger->debug("Done getting schema updates.");
    SchemaPath = RepoPath / "schemas";
  }

  return SchemaPath.string();
}
