<?php
// $Id$

/**
 * GameQ plugin.
 */
class gameservers_query_gameq extends gameservers_query {

  public function getGameTypes() {
    gameservers_include_library('GameQ.php', 'gameq');
    //$library_path = gameservers_get_path_library('gameq');

    $types = array();
    $ini = parse_ini_file(GAMEQ_BASE .'games.ini', TRUE);
    foreach ($ini as $key => $entry) {
      $types[$key] = $entry['name'];
    }

    return $types;
  }

  protected function queryServer($server, &$response) {
    gameservers_include_library('GameQ.php', 'gameq');

    $gq = new GameQ();
    $gq->addServer($server->id, array($server->config['query']['gametype'], $server->hostname, $server->port));
    //$gq->setOption('timeout', 200);

    // Send requests, and parse the data
    $gq->setFilter('normalise');
    //$gq->setFilter('sortplayers', 'name');

    $results = $gq->requestData();

    if (isset($results[$server->id])) {
      $data = $results[$server->id];

      $response['raw'] = $data;

      $response['address']    = $data['gq_address'];
      $response['port']       = $data['gq_port'];
      $response['online']     = $data['gq_online'] ? TRUE : FALSE;
      $response['protocol']   = $data['gq_prot'];

      $response['game']       = $data['gq_mod'];
      $response['servername'] = $data['gq_hostname'];
      $response['mapname']    = $data['gq_mapname'];

      $response['numplayers'] = $data['gq_numplayers'];
      $response['maxplayers'] = $data['gq_maxplayers'];

      $response['extra'] = array();
      foreach ($data as $key => $value) {
        if (strpos($key, 'gq_') !== 0) {
          $response['extra'][$key] = $value;
        }
      }

      if (!empty($data['players'])) {
        foreach ($data['players'] as $player_info) {
          if ($name = $player_info['name']) {
            $player = new stdClass();
            $player->name = check_plain($name);
            $player->score = 0;
            $player->time = 0;

            $response['players'][] = $player;
          }
        }
      }

      return TRUE;
    }

    return FALSE;
  }
}
