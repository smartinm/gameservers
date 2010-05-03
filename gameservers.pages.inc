<?php
// $Id$

/**
 * @file
 * Admin page callbacks for the gameservers module.
 */

/**
 * Menu callback; display a list of game servers.
 */
function gameservers_browse() {
  // Try to allocate enough time to query all the servers.
  if (function_exists('set_time_limit')) {
    @set_time_limit(240);
  }

  $rows = array();
  $result = db_query('SELECT id FROM {gameservers_server} WHERE status = 1');
  while ($data = db_fetch_object($result)) {
    $server = gameservers_server_load($data->id);
    $query = gameservers_get_query_instance($server->config['query']['plugin']);
    $response = $query->getResponse($server);

    $response = (object) $response;
    $join_link  = l(t('!hostname:!port', array('!hostname' => $server->hostname, '!port' => $server->port)), '');
    $detail_link = l(t('details'), "gameservers/$reponse->id");
    $row = array(
      !empty($response->online) ? t('Online') : t('Offline'),
      check_plain($response->protocol) .'/'. check_plain($response->game),
      $join_link,
      check_plain($response->servername),
      check_plain($response->mapname),
      check_plain($response->numplayers) .'/'. check_plain($response->maxplayers),
      $detail_link,
    );
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No game servers available.'), 'colspan' => (7)));
  }

  $output = theme('table', array(), $rows);

  //drupal_set_title(t('Game servers list'));
  return $output;
}
