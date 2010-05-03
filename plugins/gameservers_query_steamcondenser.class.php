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
      'goldsrc' => 'GoldSrc game (HL, CS, TFC, ...)',
      'source' => 'Source game (HL2, CS:S, TF2, L4D, ...)',
    );
    return $types;
  }

  public function query($config) {
    $lib_path = gameservers_get_path_library('steam-condenser') .'/lib';
    set_include_path(get_include_path() . PATH_SEPARATOR . $lib_path);
    gameservers_include_library('lib/steam/servers/SourceServer.php', 'steam-condenser');

    $error_level = error_reporting(E_ALL & ~E_USER_NOTICE);
    try {
      $output = '';

      $server = new SourceServer(new InetAddress($server_address[0]), $server_address[1]);
      $server->initialize();
      $server->updatePlayerInfo();
      $server->updateRulesInfo();
      dpm($server->getServerInfo());

      $server_info = array();
      foreach ($server->getServerInfo() as $key => $value) {
        $server_info[] = '<strong>'. $key .'</strong>: '. check_plain($value);
      }
      //$output = '<pre>' . $server->__toString() .'</pre>';
      $output = theme('item_list', $server_info);
    }
    catch (Exception $ex) {
      drupal_set_message($ex->__toString(), 'error');
    }

    error_reporting($error_level);

    /*
    $data['raw'] = $response;
    $data['game_link'] = $response['b']['ip'] .':'. $response['b']['c_port'];
    $data['game_name'] = $response['s']['name'];
    $data['map_image'] = '';
    $data['map_name'] = $response['s']['map'];
    $data['num_players'] = $response['s']['players'] .'/'. $response['s']['playersmax'];

    $data['players'] = array();
    if (!empty($response['p'])) {
      foreach ($response['p'] as $player) {
        if ($player['name']) {
          $name = $player['name'];
          $data['players'][$name]->name = check_plain($name);
        }
      }
    }
    */
    return $data;
  }
}
