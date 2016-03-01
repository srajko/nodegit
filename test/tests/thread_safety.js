var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("ThreadSafety", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repo) {
        test.repository = repo;
        return repo.openIndex();
      })
      .then(function(index) {
        test.index = index;
      });
  });

  it("can enable and disable thread safety", function() {
    var originalValue = NodeGit.getThreadSafetyStatus();

    NodeGit.enableThreadSafety();
    assert.equal(2, NodeGit.getThreadSafetyStatus());

    NodeGit.setThreadSafetyStatus(1);
    assert.equal(1, NodeGit.getThreadSafetyStatus());

    NodeGit.setThreadSafetyStatus(0);
    assert.equal(0, NodeGit.getThreadSafetyStatus());

    NodeGit.setThreadSafetyStatus(originalValue);
  });

  it("can lock something and cleanup mutex", function() {
    var diagnostics = NodeGit.getThreadSafetyDiagnostics();
    var originalCount = diagnostics.storedMutexesCount;
    // call a sync method to guarantee that it stores a mutex,
    // and that it will clean up the mutex in a garbage collection cycle
    this.repository.headDetached();

    diagnostics = NodeGit.getThreadSafetyDiagnostics();
    switch(NodeGit.getThreadSafetyStatus()) {
      case 2:
        // this is a fairly vague test - it just tests that something
        // had a mutex created for it at some point (i.e., the thread safety
        // code is not completely dead)
        assert.ok(diagnostics.storedMutexesCount > 0);
        break;
      case 1:
        assert.equal(originalCount, diagnostics.storedMutexesCount);
        break;

      case 0:
        assert.equal(0, diagnostics.storedMutexesCount);
    }
  });
});
