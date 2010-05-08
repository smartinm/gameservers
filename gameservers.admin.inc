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
    '#title' => t('Server address'),
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
    '#tree' => FALSE,
  );

  $form['advanced']['hostname'] = array(
    '#type' => 'textfield',
    '#title' => t('IP or hostname'),
    '#description' => t('The IP address or hostname of this game server.'),
    '#required' => TRUE,
  );

  $form['advanced']['port'] = array(
    '#type' => 'textfield',
    '#title' => t('Port'),
    '#size' => 10,
    '#description' => t('The connect port of this game server.'),
    '#required' => TRUE,
  );

  $form['advanced']['qport'] = array(
    '#type' => 'textfield',
    '#title' => t('Query port'),
    '#size' => 10,
    '#description' => t('The query port of this game server, the query library will try to guess. Leave it blank unless you know what youre doing.'),
  );

  $form['advanced']['status'] = array(
    '#type' => 'radios',
    '#title' => t('Status'),
    '#default_value' => 1,
    '#options' => array(t('Disabled'), t('Enabled'))
  );

  $plugins = array('' => t('- Please choose -'));
  foreach (gameservers_get_query_libraries() as $plugin => $data) {
    if (!$data['hidden']) {
      $plugins[$plugin] = $data['title'];
    }
  }
  $default_plugin = variable_get('gameservers_default_plugin', '');

  $form['advanced']['config']['#tree'] = TRUE;
  $form['advanced']['config']['query']['plugin'] = array(
    '#type' => 'select',
    '#title' => t('Query library'),
    '#required' => TRUE,
    '#options' => $plugins,
    '#default_value' => $default_plugin,
    '#description' => t('Select a game server query library.'),
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
function gameservers_admin_edit(&$form_state, $server) {
  $form = array();

  $form['#server'] = $server;

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
    '#description' => t('The IP address or hostname of this game server.'),
    '#required' => TRUE,
    '#default_value' => isset($server->hostname) ? $server->hostname : '',
  );

  $form['server']['port'] = array(
    '#type' => 'textfield',
    '#title' => t('Port'),
    '#size' => 8,
    '#description' => t('The connect port of this game server.'),
    '#required' => TRUE,
    '#default_value' => isset($server->port) ? $server->port : '',
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

  // -- Plugin query specific settings

  $form['config'] = array('#tree' => TRUE);
  $form['config']['query'] = array(
    '#type' => 'fieldset',
    '#title' => t('Plugin specific settings'),
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
    '#title' => t('Query library'),
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
      '#title' => t('Game type'),
      '#default_value' => isset($server->config['query']['gametype']) ? $server->config['query']['gametype'] : '',
      '#required' => TRUE,
      '#options' => array(
        '' => $default,
       ) + $gametypes,
    );

    $configuration = $query->config_form($server, $form_state);
    if (!empty($configuration)) {
      $form['config']['query'] += $configuration;
    }
  }

  // -- Plugin style specific settings
  $blocks = array('' => $default);
  foreach (ctools_get_plugins('gameservers', 'blocks') as $block => $data) {
    $blocks[$block] = $data['title'];
  }

  $form['config']['blocks'] = array(
    '#type' => 'fieldset',
    '#title' => t('Style specific settings'),
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
function gameservers_admin_configure(&$form_state) {
  $form['intro'] = array(
    '#type' => 'item',
    '#value' => t('The following option sets the default configuration in which a new game server will be added.'),
  );

  $form['plugins'] = array(
    '#type' => 'fieldset',
    '#title' => t('Plugins'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#description' => t('Choose the game server query plugin to be the default.'),
  );

  $default_plugin = variable_get('gameservers_default_plugin', '');

  include_once './includes/install.inc';
  $plugins = gameservers_get_query_libraries();
  foreach ($plugins as $key => $plugin) {
    if (!$plugin['hidden']) {
      $query = gameservers_get_query_instance($key);
      $requirements = $query->requirements();
      $form['plugins']['gameservers_default_plugin'][$key] = array(
        '#type' => 'radio',
        '#parents' => array('gameservers_default_plugin'),
        '#title' => $plugin['title'],
        '#description' => isset($plugin['description']) ? $plugin['description'] : '',
        '#return_value' => $key,
        '#default_value' => ($default_plugin == $key) ? $key : '',
        '#disabled' => drupal_requirements_severity($requirements) == REQUIREMENT_ERROR ? TRUE : FALSE,
      );

      foreach ($requirements as $requirement) {
        if (isset($requirement['severity'])) {
          $form['plugins']['plugin_key'][] = array(
            '#type' => 'markup',
            '#value' => $requirement['description'],
          );
        }
      }
    }
  }

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

  return system_settings_form($form);
}
