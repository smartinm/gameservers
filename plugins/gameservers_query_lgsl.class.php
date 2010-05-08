<?php
// $Id$

/**
 * @file
 *
 */

/**
 *
 */
class gameservers_query_lgsl extends gameservers_query {

  public function getGameTypes() {
    gameservers_include_library('lgsl_protocol.php', 'lgsl');
    $lgsl_types = lgsl_type_list();
    unset($lgsl_types['test']);
    return $lgsl_types;
  }

  protected function queryServer($server, &$response) {
    gameservers_include_library('lgsl_protocol.php', 'lgsl');

    $config = $server->config['query'];

    // s = STANDARD = info returned in the same format for all games.
    // e = EXTRA    = info that varies between games.
    // p = PLAYER   = info that is player specific.

    $data = lgsl_query_live($config['gametype'], $server->hostname, $server->port, $config['q_port'], $config['s_port'], 'sep');

    $response['raw'] = $data;

    $response['address']    = $data['b']['ip'];
    $response['port']       = $data['b']['c_port'];
    $response['online']     = $data['b']['status'] ? TRUE : FALSE;
    $response['protocol']   = $data['b']['type'];

    $response['game']       = $data['s']['game'];
    $response['servername'] = $data['s']['name'];
    $response['mapname']    = $data['s']['map'];

    $response['numplayers'] = $data['s']['players'];
    $response['maxplayers'] = $data['s']['playersmax'];

    $response['extra'] = $data['e'];

    $response['players'] = array();
    if (!empty($data['p'])) {
      foreach ($data['p'] as $player_info) {
        if (!empty($player_info['name'])) {
          $player = new stdClass();
          $player->name = check_plain($player_info['name']);
          $player->score = $player_info['score'];
          $player->time = $player_info['time'];

          $response['players'][] = $player;
        }
      }
    }

    return TRUE;
  }

  /* (non-PHPdoc)
   * @see plugins/gameservers_query#config_form($server, $form_state)
   */
  public function config_form($server, &$form_state) {
    $config = $server->config['query'];

    gameservers_include_library('lgsl_protocol.php', 'lgsl');
    $ports = lgsl_port_conversion($config['gametype'], $server->port, 0, 0);

    $form['c_port'] = array(
      '#type' => 'textfield',
      '#title' => t('Connection port'),
      '#size' => 8,
      '#value' => $server->port,
      '#disabled' => TRUE,
      '#description' => t('This is the port that is shown, passed to others, and used by the game for joining.'),
    );
    $form['q_port'] = array(
      '#type' => 'textfield',
      '#title' => t('Query port'),
      '#size' => 8,
      '#default_value' => isset($config['q_port']) ? $config['q_port'] : $ports[1],
      '#description' => t('The most important port as servers will not respond if this is wrong. For many games the connection port and query port are the same. LGSL make a guess based on the connection port.'),
    );
    $form['s_port'] = array(
      '#type' => 'textfield',
      '#title' => t('Software port'),
      '#size' => 8,
      '#default_value' => isset($config['s_port']) ? $config['s_port'] : $ports[2],
      '#description' => t('Most of the time this should be left blank or zero, it will be set when needed. There are games with two query ports, so this option is used when the \'Software Link\' needs a port different from the LGSL connection and query port.'),
    );

    return $form;
  }

  public function requirements() {
    $requirements = array();

    $requirements['lgsl_library'] = array(
      'title' => t('LGSL Library'),
      'value' => t('Found'),
      'severity' => REQUIREMENT_INFO,
    );

    $path = gameservers_get_path_library('lgsl');
    if (file_exists($path .'/lgsl_protocol.php')) {
      gameservers_include_library('lgsl_protocol.php', 'lgsl');
    }

    if (!function_exists('lgsl_query_live')) {
      $requirements['lgsl_library']['description'] = t('Your PHP installation is too old. Drupal requires at least PHP %version.', array('%version' => DRUPAL_MINIMUM_PHP));
      $requirements['lgsl_library']['severity'] = REQUIREMENT_ERROR;
    }

    return $requirements;
  }
}
