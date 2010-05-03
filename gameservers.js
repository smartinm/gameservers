// $Id$

/**
 *
 */
Drupal.behaviors.gameServersAddForm = function(context) {
  var hostInput = $('#edit-hostname');
  var portInput = $('#edit-port');
  var serverPattern = /([a-z0-9\.]+):(\d+)/;
  $('#edit-server').change(function() {
    var match = serverPattern.exec($(this).val());
    if (match) {
      hostInput.val(match[1]);
      portInput.val(match[2]);
    }
  });
};
