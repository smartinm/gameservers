<?php
// $Id$

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Game server query'),
  'description' => t('Abstract class, defines interface for game server query plugins.'),
  'hidden' => TRUE,
  'handler' => array(
    'class' => 'gameservers_query',
  ),
);
