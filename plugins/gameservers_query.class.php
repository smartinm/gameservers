<?php
// $Id$

/**
 * @file
 * Definition of gameservers_query abstract class.
 */

/**
 * Abstract class, defines interface for game server query libraries.
 */
abstract class gameservers_query {

  /**
   * Consulta un servidor de juego específico.
   *
   * @return
   *   TRUE si el servidor ha respondido correctamente, FALSE en caso contrario.
   */
  protected abstract function queryServer($server, &$response);

  /**
   *
   */
  public function getResponse($server) {
    // Default values
    $response = array(
      // Query info
      'address'     => $server->hostname,
      'port'        => $server->port,
      'online'      => FALSE,
      'protocol'    => '',
      'raw'         => NULL,
      // Standard response info
      'game'        => '',
      'servername'  => '',
      'mapname'     => '',
      'password'    => FALSE,
      // Players/Teams response info
      'numplayers'  => 0,
      'maxplayers'  => 0,
      'players'     => array(),
      'teams'       => array(),
      // Extra info
      'extra'       => array(),
    );

    if ($this->queryServer($server, $response)) {
      // @todo image maps
      $mapname = $response['mapname'];
      $game = $response['game'];
      $image_map = "http://image.www.gametracker.com/images/maps/160x120/$game/$mapname.jpg";
      $response['mapimage'] = theme('image', $image_map, $mapname, $mapname, NULL, FALSE);
    }

    return $response;
  }

  /**
   * Declara los tipos de juegos que soporta la biblioteca de consulta.
   *
   * @return
   *   Un array de tipos de juegos.
   *
   *   Example:
   *   @code
   *   array(
   *     'bfbc2' => 'Battlefield Bad Company 2',
   *     'halflife' => 'Half-Life',
   *     'source' => 'Source game',
   *   )
   *   @endcode
   */
  public abstract function getGameTypes();

  /**
   * Return default configuration.
   *
   * @return
   *   Array where keys are the variable names of the configuration elements and
   *   values are their default values.
   */
  public function config_defaults($server) {
    return array();
  }

  /**
   * Return configuration form for this object. The keys of the configuration
   * form must match the keys of the array returned by getConfigDefaults().
   *
   * @return
   *   FormAPI style form definition.
   */
  public function config_form($server, &$form_state) {
    return array();
  }

  /**
   * Validation handler for configForm().
   *
   * Set errors with form_set_error().
   *
   * @param $values
   *   An array that contains the values entered by the user through configForm.
   */
  public function config_form_validate($server, &$values) {
  }

  /**
   * Check library requirements.
   *
   * @see hook_requirements()
   */
  public function requirements() {
    return array();
  }
}
