<?php
// $Id$

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('GameQ'),
  'description' => t('GameQ is a library to query one or more gameservers using UDP and return the formatted results in an array. Currently over 170 games  are supported.'),
  'home_url' => 'http://gameq.sourceforge.net/',
  'download_url' => 'http://sourceforge.net/projects/gameq/files/',
  'requirements' => array('php5', 'udp'),
  'handler' => array(
    'class' => 'gameservers_query_gameq',
    'parent' => 'gameservers_query',
  ),
);
