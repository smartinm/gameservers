<?php
// $Id$
/**
 * @file gameservers-lgsl.tpl.php
 * Template for a LGSL's style block.
 *
 * Available variables:
 * - $picture: Image configured for the account linking to the users page.
 * - $profile: Keyed array of all profile fields that have a value.
 *
 * Each $field in $profile contains:
 * - $field->title: Title of the profile field.
 * - $field->value: Value of the profile field.
 * - $field->type: Type of the profile field, i.e., checkbox, textfield,
 *   textarea, selection, list, url or date.
 *
 * @see template_preprocess_gameservers_block()
 */
?>
<div class="global">
  <div class="header_footer">
    <div class="item_float_left">
      <a href="http://www.gametracker.com/search/tf2/ES/" target="_blank"><img src="http://image.www.gametracker.com/images/flags/es.gif" alt="ES" title="Spain" /></a>&nbsp;
    </div>
    <div class="item_float_right">
      &nbsp;<a href="http://www.gametracker.com/search/tf2/" target="_blank"><img src="http://image.www.gametracker.com/images/search/game_icons/tf2.png" alt="Team Fortress 2" title="Team Fortress 2"/></a>
    </div>
    <div class="item_float_right"><a href="http://www.gametracker.com/search/tf2/" target="_blank">Team Fortress 2</a></div>
    <div class="item_float_clear"></div>
  </div>

  <div class="server_name"><?php print $response->servername; ?></div>
  <div class="info_line">
    <div class="item_float_left">
      <b>IP:</b>
    </div>
    <div class="item_float_right">
      <img src="http://cache.www.gametracker.com/images/components/html0/online.gif" alt="Online" />
      <a href="http://www.gametracker.com/server_info/91.151.98.154:27035/" target="_blank">91.151.98.154:27035</a>
    </div>
    <div class="item_float_clear"></div>
  </div>

  <div class="info_line">
    <div class="item_float_left"><b>Players:</b></div>
    <div class="item_float_right"><?php print $response->numplayers . '/' . $response->maxplayers; ?></div>
    <div class="item_float_clear"></div>
  </div>

  <div class="info_line">
    <div class="item_float_left"><b>Rank:</b></div>
    <div class="item_float_right">168th (95th pctile)</div>
    <div class="item_float_clear">
    </div>

  </div>
  <div class="info_line">
    <div class="item_float_left"><b>Map:</b></div>
    <div class="info_line_right"><a href="#" target="_blank"><?php print $response->mapname; ?></a></div>
    <div class="item_float_clear"></div>
  </div>

  <div class='map_image'><?php print $response->mapimage; ?></div>

  <div class="info_line"><b>Online Players:</b></div>
  <div class="scrollable channelViewScrollable">
    <?php foreach ($response->players as $player) : ?>
    <div class="scrollable_on_c01">1</div>
    <div class="scrollable_on_c02"><a href="#" target="_blank" ><?php print $player->name; ?></a></div>
    <div class="scrollable_on_c03">1</div>
    <div class="item_float_clear"></div>
    <?php endforeach; ?>
  </div>
  <div style="text-align:center; padding:2px 0px 6px 0px;">
    <span class="btn_custom" onclick="javascript:getElementById('popuptest').style.display = 'block'; return false;">
      JOIN THIS SERVER
    </span>
  </div>
  <div class="header_footer">

    <div class="item_float_right">
      &nbsp;
      <a href="http://www.gametracker.com" target="_blank">
        <img src="http://cache.www.gametracker.com/images/components/html0/gt_icon.gif" alt="GameTracker" />
      </a>
    </div>
    <div class="item_float_right">
      <a href="http://www.gametracker.com" target="_blank">
      GameTracker.com
      </a>

    </div>
    <div class="item_float_clear"></div>
  </div>
</div>
