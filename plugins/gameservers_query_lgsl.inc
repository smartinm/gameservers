<?php
// $Id$

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('LGSL'),
  'description' => t('Live Game Server List (LGSL) is a PHP Game Server Status Library that can query the status of many online games, is easy to use, and tries to return data in a consistent format.'),
  'home_url' => 'http://www.greycube.com/site/news.php',
  'download_url' => 'http://www.greycube.com/site/download.php?list.10',
  'handler' => array(
    'class' => 'gameservers_query_lgsl',
    'parent' => 'gameservers_query',
  ),
);

// http://www.greycube.com/help/readme/lgsl/