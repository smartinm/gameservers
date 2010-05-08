<?php
// $Id$

/**
 * @file
 *
 */

/**
 *
 */
class gameservers_query_steamcondenser extends gameservers_query {

  public function getGameTypes() {
    $types = array(
      'goldsrc' => 'GoldSrc game',
      'source' => 'Source game',
    );
    return $types;
  }

  protected function queryServer($server, &$response) {
    $error_level = error_reporting(E_ALL & ~E_USER_NOTICE);

    $lib_path = gameservers_get_path_library('steam-condenser') .'/lib';
    set_include_path(get_include_path() . PATH_SEPARATOR . $lib_path);
    require_once 'steam/servers/SourceServer.php';

    try {
      $config = $server->config['query'];
      if ($config['gametype'] == 'goldsrc') {
        $server = new GoldSrcServer(new InetAddress($server->hostname), $server->port);
      }
      else {
        $server = new SourceServer(new InetAddress($server->hostname), $server->port);
      }

      $server->initialize();

      $response['raw'] = array(
        'server' => $server->getServerInfo(),
        'players' => $server->getPlayers(),
        'rules' => $server->getRules(),
        'ping' => $server->getPing(),
      );

      $response['address']    = $server->hostname;
      $response['port']       = $response['raw']['server']['serverPort'];
      $response['online']     = $response['raw']['ping'] ? TRUE : FALSE;
      $response['protocol']   = $config['gametype'];

      $response['game']       = $response['raw']['server']['gameDir'];
      $response['servername'] = $response['raw']['server']['serverName'];
      $response['mapname']    = $response['raw']['server']['mapName'];

      $response['numplayers'] = $response['raw']['server']['playerNumber'];
      $response['maxplayers'] = $response['raw']['server']['maxPlayers'];

      $response['extra'] = $server->getRules();

      foreach ($server->getPlayers() as $steam_player) {
        $player = new stdClass();
        $player->name = check_plain($steam_player->getName());
        $player->score = $steam_player->getScore();
        $player->time = $steam_player->getConnectTime();

        $response['players'][] = $player;
      }
    }
    catch (Exception $ex) {
      drupal_set_message($ex->__toString(), 'error');
    }

    error_reporting($error_level);

    return TRUE;
  }
}
