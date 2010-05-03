<?php
// $Id$

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Steam Condenser'),
  'description' => t('The Steam Condenser is a multi-language library for querying the Steam Community, Source and GoldSrc game servers as well as the Steam master servers.'),
  'help' => t('Download Steam Condenser and place in the directory /steam-condenser into Libraries module\'s search path (for instance sites/all/libraries) or gameservers/libraries.'),
  'home_url' => 'http://koraktor.github.com/steam-condenser/',
  'download_url' => 'http://koraktor.github.com/steam-condenser/downloads.html',
  'handler' => array(
    'class' => 'gameservers_query_steamcondenser',
    'parent' => 'gameservers_query',
  ),
);
