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
<div class="game-info">
  <div class="game-link"><?php print $response->servername; ?></div>
  <div class="game-name" title="<?php print $response->servername; ?>"><?php print $response->servername; ?></div>
</div>
<div class="map-info">
  <div class="map-image"><?php print $response->mapimage; ?></div>
  <div class="map-name"><?php print $response->mapname; ?></div>
</div>
<div class="players">
  <span class="title"><?php print t('Players'); ?></span>
  <span class="num"><?php print $response->numplayers . '/' . $response->maxplayers; ?></span>
  <?php foreach ($response->players as $player) : ?>
  <div class="player" title="<?php print $player->name; ?>">
    <?php print $player->name; ?>
  </div>
  <?php endforeach; ?>
</div>