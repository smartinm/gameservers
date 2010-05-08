<?php
// $Id$

/**
 * @file
 * @todo Add documentation.
 */

/**
 *
 */
function gameservers_block_list() {
  $blocks = array();
  $result = db_query('SELECT id, name FROM {gameservers_server} WHERE status = 1');
  while ($server = db_fetch_object($result)) {
    $blocks[$server->id] = array(
      'info' => t('Game Server: !name', array('!name' => $server->name)),
      'cache' => BLOCK_CACHE_PER_ROLE | BLOCK_CACHE_PER_PAGE,
    );
  }
  return $blocks;
}

/**
 *
 */
function gameservers_block_configure($id) {
}

/**
 *
 */
function gameservers_block_save($id, $edit) {
}

/**
 *
 */
function gameservers_block_view($id) {
  if ($server = gameservers_server_load($id)) {
    if ($server->status) {
      return array(
        'subject' => t('Game Server'),
        'content' => gameservers_server_view($server, TRUE),
      );
    }
  }
}
