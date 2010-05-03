<?php
// $Id$

/**
 * @file
 * Admin page callbacks for the gameservers module.
 */

/**
 * Menu callback for admin/build/gameservers.
 */
function gameservers_admin_overview() {
  // @todo: Add the filter form above the overview table.
  //$output = drupal_get_form('path_admin_filter_form', $keys);

  $plugins = gameservers_get_query_libraries();
  $destination = drupal_get_destination();

  $header = array(
    array('data' => t('Name'), 'field' => 'name', 'sort' => 'asc'),
    array('data' => t('Address'), 'field' => 'hostname'),
    //array('data' => t('Port'), 'field' => 'port'),
    array('data' => t('Plugin')),
    array('data' => t('Type')),
    array('data' => t('Status'), 'field' => 'status'),
    array('data' => t('Operations'), 'colspan' => '2')
  );

  $sql = 'SELECT * FROM {gameservers_server}';
  $sql .= tablesort_sql($header);
  $result = db_query($sql);

  $rows = array();
  while ($data = db_fetch_object($result)) {
    $config = unserialize($data->config);
    $edit_link = l(t('edit'), "admin/build/gameservers/edit/$data->id", array('query' => $destination));
    $delete_link = l(t('delete'), "admin/build/gameservers/delete/$data->id", array('query' => $destination));
    $row = array(
      check_plain($data->name),
      check_plain($data->hostname) .':'. check_plain($data->port),
      check_plain($plugins[$config['query']['plugin']]['title']),
      check_plain($config['query']['gametype']),
      $data->status ? t('Enabled') : t('Disabled'),
      $edit_link, $delete_link,
    );
    $rows[] = $row;
  }

  if (empty($rows)) {
    $empty_message = $keys ? t('No game servers found.') : t('No game servers available.') ;
    $rows[] = array(array('data' => $empty_message, 'colspan' => (7)));
  }

  return theme('table', $header, $rows);
}

/**
 * Add game server form.
 */
function gameservers_admin_add_form(&$form_state) {
  drupal_add_js(gameservers_get_path('gameservers.js'));

  $form = array();

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#maxlength' => 255,
    '#required' => TRUE,
    '#description' => t('The name of this game server.'),
  );

  $form['server'] = array(
    '#type' => 'textfield',
    '#title' => t('Address'),
    '#description' => t('The game server address (e.g. <em>192.0.32.10:27015)</em>.'),
  );

  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('An optional description or comment of this game server.'),
  );

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['advanced']['hostname'] = array(
    '#type' => 'textfield',
    '#title' => t('IP address'),
    '#description' => t('The IP or hostname of this game server.'),
    '#required' => TRUE,
  );

  $form['advanced']['port'] = array(
    '#type' => 'textfield',
    '#title' => t('Port'),
    '#size' => 8,
    '#description' => t('The connect port of this game server.'),
    '#required' => TRUE,
  );

  $form['advanced']['qport'] = array(
    '#type' => 'textfield',
    '#title' => t('Query port'),
    '#size' => 8,
    '#description' => t('The query port of this game server.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add server'),
  );

  return $form;
}

/**
 * Validation handler for gameservers_admin_add_form().
 */
function gameservers_admin_add_form_validate($form, &$form_state) {
}

/**
 * Submit handler for gameservers_admin_add_form().
 */
function gameservers_admin_add_form_submit($form, &$form_state) {
  // Create game server.
  $server = (object) $form_state['values'];

  gameservers_server_save($server);

  drupal_set_message(t('Your game server has been created with default settings. If they do not fit your use case you can adjust them here.'));
  $form_state['redirect'] = 'admin/build/gameservers/edit/'. $server->id;
}

/**
 * Delete game server form.
 */
function gameservers_admin_delete_form(&$form_state, $id) {
  $form['#redirect'] = 'admin/build/gameservers';
  $form['#server'] = gameservers_server_load($id);

  return confirm_form($form,
    t('Are you sure you want to delete the game server !name?', array('!name' => $form['#server']->name)),
    $form['#redirect'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler for gameservers_admin_delete_form().
 */
function gameservers_admin_delete_form_submit($form, &$form_state) {
  gameservers_server_delete($form['#server']->id);
  drupal_set_message(t('The game server !name has been removed.', array('!name' => $form['#server']->name)));
}

/**
 * Menu callback; handles pages for editing game servers.
 *
 * @see feeds_ui_edit_page()
 */
function gameservers_admin_edit_page2($id, $active = 'help', $plugin_key = '') {

  $server = gameservers_server_load($id);

  // Get plugins and configuration.
  $query_libraries = gameservers_get_query_libraries();
  $config = $server->config;

  // Base path for changing the active container.
  $path = 'admin/build/gameservers/edit/'. $server->id;

  $active_container = array(
    'class' => 'active-container',
    'actions' => array(l(t('Help'), $path)),
  );

  switch ($active) {
    case 'help':
      $active_container['title'] = t('Getting started');
      $active_container['body'] = '<div class="help feeds-admin-ui">'. feeds_ui_edit_help() .'</div>';
      unset($active_container['actions']);
      break;

    case 'query':
      $active_container['title'] = t('Select a !plugin_type', array('!plugin_type' => $active));
      $active_container['body'] = drupal_get_form('gameservers_admin_plugin_form', $server, $active);
      break;

    case 'settings':
      //drupal_add_js(drupal_get_path('module', 'feeds_ui') .'/feeds_ui.js');

      if (empty($plugin_key)) {
        $active_container['title'] = t('Basic settings');
        $active_container['body'] = feeds_get_config_form($importer);
      }
      elseif (in_array($plugin_key, array_keys($plugins)) && $plugin = feeds_plugin_instance($plugin_key, $importer->id)) {
        $active_container['title'] = t('Settings for !plugin', array('!plugin' => $plugins[$plugin_key]['name']));
        $active_container['body'] = feeds_get_config_form($plugin);
      }
      break;
  }

  // Build config info.
  $config_info = $info = array();
  $info['class'] = 'config-set';

  // Basic information.
  $items = array();
  $items[] = t('Demo');

  $info['title'] = t('Basic settings');
  $info['body'] = array(array(
    'body' => theme('item_list', $items),
    'actions' => array(l(t('Settings'), $path .'/settings')),
  ));
  $config_info[] = $info;

  // Query
  $fetcher = $plugins[$config['fetcher']['plugin_key']];
  $actions = array();
  if (feeds_get_config_form($importer->fetcher)) {
    $actions = array(l(t('Settings'), $path .'/settings/'. $config['fetcher']['plugin_key']));
  }
  $info['title'] = t('Fetcher');
  $info['body'] = array(array(
    'title' => $fetcher['name'],
    'body' => $fetcher['description'],
    'actions' => $actions,
  ));
  $info['actions'] = array(l(t('Change'), $path .'/fetcher'));
  $config_info[] = $info;

  return theme('feeds_ui_edit_page', $config_info, $active_container);
}

/**
 * Build a form of plugins to pick from.
 *
 * @param $form_state
 *   Form API form state array.
 * @param $importer
 *   FeedsImporter object.
 * @param $type
 *   Plugin type. One of 'fetcher', 'parser', 'processor'.
 *
 * @return
 *   A Form API form definition.
 */
function gameservers_admin_plugin_form(&$form_state, $server, $type) {
  $plugins = feeds_get_plugins_by_type($type);

  $form = array();
  $form['#importer'] = $importer;
  $form['#plugin_type'] = $type;

  foreach ($plugins as $key => $plugin) {
    $form['plugin_key'][$key] = array(
      '#type' => 'radio',
      '#parents' => array('plugin_key'),
      '#title' => $plugin['name'],
      '#description' => isset($plugin['help']) ? $plugin['help'] : $plugin['description'],
      '#return_value' => $key,
      '#default_value' => ($plugin['handler']['class'] == get_class($importer->$type)) ? $key : '',
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#attributes' => array('class' => 'feeds-ui-hidden-submit'),
  );
  return $form;
}

/**
 * Submit handler for gameservers_admin_plugin_form().
 */
function gameservers_admin_plugin_form_submit($form, &$form_state) {
  // Set the plugin and save feed.
  $form['#importer']->setPlugin($form_state['values']['plugin_key']);
  $form['#importer']->save();
  drupal_set_message(t('Changed !type plugin.', array('!type' => $form['#plugin_type'])));
}

/**
 * Theme gameservers_admin_plugin_form().
 */
function theme_gameservers_admin_plugin_form($form) {
  drupal_add_js(drupal_get_path('module', 'feeds_ui') .'/feeds_ui.js');
  $output = '';

  foreach (element_children($form['plugin_key']) as $key) {

    // Assemble container, render form elements.
    $container = array(
      'title' => $form['plugin_key'][$key]['#title'],
      'body' => $form['plugin_key'][$key]['#description'],
    );
    $form['plugin_key'][$key]['#title'] = t('Select');
    $form['plugin_key'][$key]['#attributes']['class'] = 'feeds-ui-radio-link';
    unset($form['plugin_key'][$key]['#description']);
    $container['actions'] = array(drupal_render($form['plugin_key'][$key]));

    $output .= theme_feeds_ui_container($container);
  }

  $output .= drupal_render($form);
  return $output;
}


/**
 * Menu callback; handles pages for creating and editing game servers.
 */
function gameservers_admin_edit_page($id) {
  if ($server = gameservers_server_load($id)) {
    drupal_set_title(t("'%name' game server", array('%name' => $server->name)));
    return drupal_get_form('gameservers_admin_edit', $server);
  }
  drupal_not_found();
}

/**
 * Menu callback; displays the block configuration form.
 */
function gameservers_admin_edit(&$form_state, $server = NULL) {
  $form = array();

  $form['id'] = array(
    '#type' => 'value',
    '#value' => isset($server->id) ? $server->id : '',
  );

  $form['server'] = array(
    '#type' => 'fieldset',
    '#title' => t('Server information'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['server']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#maxlength' => 255,
    '#required' => TRUE,
    '#description' => t('The name of this game server.'),
    '#default_value' => isset($server->name) ? $server->name : '',
  );

  $form['server']['hostname'] = array(
    '#type' => 'textfield',
    '#title' => t('IP or hostname'),
    '#description' => t('The IP or hostname of this game server.'),
    '#required' => TRUE,
    '#default_value' => isset($server->hostname) ? $server->hostname : '',
    //'#prefix' => '<div class="container-inline">',
  );

  $form['server']['port'] = array(
    '#type' => 'textfield',
    '#title' => t('Port'),
    '#size' => 8,
    '#description' => t('The connect port of this game server.'),
    '#required' => TRUE,
    '#default_value' => isset($server->port) ? $server->port : '',
    //'#suffix' => '</div>',
  );

  $form['server']['qport'] = array(
    '#type' => 'textfield',
    '#title' => t('Query port'),
    '#size' => 8,
    '#description' => t('The query port of this game server. Leave it blank if you are unsure, the query plugin will try to guess.'),
    '#default_value' => isset($server->qport) ? $server->qport : '',
  );

  $form['server']['status'] = array(
    '#type' => 'radios',
    '#title' => t('Status'),
    '#default_value' => isset($server->status) ? $server->status : 1,
    '#options' => array(t('Disabled'), t('Enabled'))
  );

  $form['server']['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('An optional description or comment of this game server.'),
    '#default_value' => isset($server->description) ? $server->description : '',
  );

  $form['config'] = array('#tree' => TRUE);
  $form['config']['query'] = array(
    '#type' => 'fieldset',
    '#title' => t('Query plugin settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $default = t('- Please choose -');

  $libraries = array('' => $default);
  foreach (gameservers_get_query_libraries() as $plugin => $data) {
    if (!$data['hidden']) {
      // @todo Sólo mostrar los plugins que están instalados
      $libraries[$plugin] = $data['title'];
    }
  }
  $query_library = isset($server->config['query']['plugin']) ? $server->config['query']['plugin'] : '';

  $form['config']['query']['plugin'] = array(
    '#type' => 'select',
    '#title' => t('Query Library'),
    '#required' => TRUE,
    '#options' => $libraries,
    '#default_value' => $query_library,
    '#description' => t('Select a game server query library.'),
  );

  if (!empty($query_library)) {
    $form['config']['query']['plugin']['#disabled'] = TRUE;

    $query = gameservers_get_query_instance($query_library);
    $gametypes = $query->getGameTypes();

    $form['config']['query']['gametype'] = array(
      '#type' => 'select',
      '#title' => t('Game Type'),
      '#default_value' => isset($server->config['query']['gametype']) ? $server->config['query']['gametype'] : '',
      '#required' => TRUE,
      '#options' => array(
        '' => $default,
       ) + $gametypes,
    );

    $configuration = $query->configForm($server, $form_state);
    if (!empty($configuration)) {
      $form['config']['query'] += $configuration;
    }

    $blocks = array('' => $default);
    foreach (ctools_get_plugins('gameservers', 'blocks') as $block => $data) {
      $blocks[$block] = $data['title'];
    }

    $form['config']['blocks'] = array(
      '#type' => 'fieldset',
      '#title' => t('Display settings'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['config']['blocks']['plugin'] = array(
      '#type' => 'select',
      '#title' => t('Block style'),
      '#required' => TRUE,
      '#options' => $blocks,
      '#default_value' => isset($server->config['blocks']['plugin']) ? $server->config['blocks']['plugin'] : '',
      '#description' => t('Select a game server block style.'),
    );
  }

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  if (isset($server->id)) {
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('gameservers_admin_server_delete'),
    );
  }

  return $form;
}

function gameservers_admin_edit_validate($form, &$form_state) {
}

function gameservers_admin_edit_submit($form, &$form_state) {
  $server = (object) $form_state['values'];
  gameservers_server_save($server);
  $form_state['redirect'] = 'admin/build/gameservers';
}

/**
 * Menu callback; Build the form presenting menu configuration options.
 */
function gameservers_admin_configure() {
  $form['intro'] = array(
    '#type' => 'item',
    '#value' => t('The following option sets the default configuration in which a new game server will be added.'),
  );

  $form['map_images'] = array(
    '#type' => 'fieldset',
    '#title' => t('Map images'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['map_images']['map_images_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Map images path'),
    '#default_value' => variable_get('map_images_path', 'maps'),
    '#size' => 30,
    '#maxlength' => 255,
    '#description' => t('Subdirectory in the directory %dir where pictures will be stored.', array('%dir' => file_directory_path() .'/gameservers')),
  );

  $plugins = array('' => t('- None -'));
  foreach (gameservers_get_query_libraries() as $plugin => $data) {
    if (!$data['hidden']) {
      $plugins[$plugin] = $data['title'];
    }
  }

  $form['gameservers_default_query_plugin'] = array(
    '#type' => 'select',
    '#title' => t('Default query plugin'),
    '#default_value' => variable_get('gameservers_default_query_plugin', ''),
    '#options' => $plugins,
    '#description' => t('Choose the game server query plugin to be the default.'),
  );

  return system_settings_form($form);
}
