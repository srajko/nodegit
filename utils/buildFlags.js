var fs = require("fs");
var path = require("path");

var isGitRepo = true;

module.exports = {
  debugBuild: !!process.env.BUILD_DEBUG,
  isElectron: process.env.npm_config_runtime === "electron",
  isGitRepo: isGitRepo,
  isNwjs: process.env.npm_config_runtime === "node-webkit",
  mustBuild: !!(isGitRepo || process.env.BUILD_DEBUG || process.env.BUILD_ONLY)
};
