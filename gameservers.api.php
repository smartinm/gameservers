<?php
// $Id$

/**
 * @file
 * API documentation for Game servers module.
 */

/**
 * Invoked before a game server will be queried.
 *
 * @param $server
 *  Game server object that has been queried.
 */
function hook_gameservers_pre_query($server) {
}

/**
 * Invoked after a game server has been queried.
 *
 * @param $server
 *  Game server object that has been queried.
 * @param $response
 *  Query response object.
 */
function hook_gameservers_reponse_query($server, $response) {
}
