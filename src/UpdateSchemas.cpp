#include "UpdateSchemas.h"
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

std::string updateSchemas(std::string SchemaRepositoryPath) {
  git_libgit2_init();
  std::shared_ptr<spdlog::logger> Logger = spdlog::get("LOG");

  const std::string RepoURL = "git://github.com/ess-dmsc/streaming-data-types";
  const std::string RepoPath = "streaming-data-types/";

  Logger->info("Updating schema information...");

  git_repository *Repo = tryRepositoryClone(Logger, RepoURL, RepoPath);

  int Error = tryRepositoryPull(Repo);

  cleanupGit(Repo);

  if (Error != 0) {
    Logger->warn("Could not get up-to-date schema information, falling back on "
                 "possibly-outdated schemas.");
    return "schemas/";
  }

  Logger->info("Done getting schema updates.");
  return RepoPath + "schemas/";
}
