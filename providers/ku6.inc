<?php

/**
 * @file
 *  This is an ku6 provider include file for Embedded Media Video.
 *  Use this as a base for creating new provider files.
 *
 *  When using this, first make the following global replacements:
 *    * Replace ku6 with the name of your provider in all caps.
 *    * Replace ku6 with the name of your provider in all lower case.
 *    * Replace ku6 with the name (to be translated) of your provider in
 *        uppercase.
 *
 *  You then need to go through each function and modify according to the
 *  requirements of your provider's API.
 */

/**
 *  This is the main URL for your provider.
 */
define('EMVIDEO_KU6_MAIN_URL', 'http://www.ku6.com/');


/**
 *  This defines the version of the content data array that we serialize
 *  in emvideo_ku6_data(). If we change the expected keys of that array,
 *  we must increment this value, which will allow older content to be updated
 *  to the new version automatically.
 */
define('EMVIDEO_KU6_DATA_VERSION', 1);

/**
 * hook emvideo_PROVIDER_info
 * This returns information relevant to a specific 3rd party video provider.
 *
 * @return
 *   A keyed array of strings requested by various admin and other forms.
 *    'provider' => The machine name of the provider. This must be the same as
 *      the base name of this filename, before the .inc extension.
 *    'name' => The translated name of the provider.
 *    'url' => The url to the main page for the provider.
 *    'settings_description' => A description of the provider that will be
 *      posted in the admin settings form.
 *    'supported_features' => An array of rows describing the state of certain
 *      supported features by the provider. These will be rendered in a table,
 *      with the columns being 'Feature', 'Supported', 'Notes'. In general,
 *      the 'Feature' column will give the name of the feature, 'Supported'
 *      will be Yes or No, and 'Notes' will give an optional description or
 *      caveats to the feature.
 */
function emvideo_ku6_info() {
  $features = array(
    array(t('Full screen mode'), t('Yes'), t('You may customize the player to enable or disable full screen playback. Full screen mode is enabled by default.')),
  );
  return array(
    'provider' => 'ku6',
    'name' => t('Ku6'),
    'url' => EMVIDEO_KU6_MAIN_URL,
    'settings_description' => t('These settings specifically affect videos displayed from !ku6.', array('!ku6' => l(t('Ku6.com'), EMVIDEO_KU6_MAIN_URL))),
    'supported_features' => $features,
  );
}

/**
 *  hook emvideo_PROVIDER_settings
 *  This should return a subform to be added to the emvideo_settings() admin
 *  settings page.
 *
 *  Note that a form field set will already be provided at $form['ku6'],
 *  so if you want specific provider settings within that field set, you should
 *  add the elements to that form array element.
 */
function emvideo_ku6_settings() {
  // We'll add a field set of player options here. You may add other options
  // to this element, or remove the field set entirely if there are no
  // user-configurable options allowed by the ku6 provider.
  $form['ku6']['player_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Embedded video player options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  // This is an option to set the video to full screen. You should remove this
  // option if it is not provided by the ku6 provider.
  $form['ku6']['player_options']['emvideo_ku6_full_screen'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow fullscreen'),
    '#default_value' => variable_get('emvideo_ku6_full_screen', 1),
    '#description' => t('Allow users to view video using the entire computer screen.'),
  );

  return $form;
}

/**
 *  hook emvideo_PROVIDER_extract
 *
 *  This is called to extract the video code from a pasted URL or embed code.
 *
 *  We'll be passed a URL or the embed code from a video when an editor pastes
 *  that in the field's textfield. We'll need to either pass back an array of
 *  regex expressions to match, or do the matching ourselves and return the
 *  resulting video code.
 *
 *  @param $parse
 *    An optional string with the pasted URL or embed code.
 *  @return
 *    Either an array of regex expressions to be tested, or a string with the
 *    video code to be used. If the hook tests the code itself, it should
 *    return either the string of the video code (if matched), or an empty
 *    array. Otherwise, the calling function will handle testing the embed code
 *    against each regex string in the returned array.
 */
function emvideo_ku6_extract($parse = '') {
  return array(
   '@v\.ku6\.com/show/([^"\&/]+)\.html@i',
   '@player\.ku6\.com/refer/([^"\&/]+)\/v.swf@i',
   '@v.ku6.com/special/show(?:_\d+)?/([^"\&/]+).html@i'
  );
}


/**
 * hook emvideo_PROVIDER_embedded_link($video_code)
 * returns a link to view the video at the provider's site.
 *  @param $video_code
 *    The string containing the video to watch.
 *  @return
 *    A string containing the URL to view the video at the original provider's site.
 */
function emvideo_ku6_embedded_link($video_code) {
  return 'http://v.ku6.com/show/'. $video_code . '.html';
}

/**
 * The embedded flash displaying the ku6 video. 
 */
function theme_emvideo_ku6_flash($item, $width, $height, $autoplay) {
  $output = '';
  if ($item['embed']) {
    $autoplay = $autoplay ? 'true' : 'false';
    $fullscreen = variable_get('emvideo_ku6_full_screen', 1) ? 'true' : 'false';
    $output = '<object width="'. $width .'" height="'. $height .'">';
    $output .= '<param name="movie" value="http://www.youku.com/v/'. $item['value'] .'">';
    $output .= '<param name="quality" value="best" />';
    $output .= '<param name="wmode" value="transparent" />';
    $output .= '<param name="allowFullScreen" value="'. $fullscreen .'" />';
    $output .= '<param name="scale" value="showAll" />';
    $output .= '<param name="FlashVars" value="autoplay='. $autoplay .'" />';    
    $output .= '<embed src="http://player.ku6.com/refer/'. $item['value'] .'/v.swf" quality="high" type="application/x-shockwave-flash" allowFullScreen="'. $fullscreen .'"  width="'. $width .'" height="'. $height .'"align="middle"></embed>';  }
    $output .= '</object>';
  return $output;
}


/**
 *  hook emvideo_PROVIDER_video
 *  This actually displays the full/normal-sized video we want, usually on the
 *  default page view.
 *  @param $embed
 *    The video code for the video to embed.
 *  @param $width
 *    The width to display the video.
 *  @param $height
 *    The height to display the video.
 *  @param $field
 *    The field info from the requesting node.
 *  @param $item
 *    The actual content from the field.
 *  @return
 *    The html of the embedded video.
 */
function emvideo_ku6_video($embed, $width, $height, $field, $item, $node, $autoplay) {
  $output = theme('emvideo_ku6_flash', $item, $width, $height, $autoplay);
  return $output;
}

/**
 *  hook emvideo_PROVIDER_video
 *
 *  This actually displays the preview-sized video we want, commonly for the
 *  teaser.
 *  @param $embed
 *    The video code for the video to embed.
 *  @param $width
 *    The width to display the video.
 *  @param $height
 *    The height to display the video.
 *  @param $field
 *    The field info from the requesting node.
 *  @param $item
 *    The actual content from the field.
 *  @return
 *    The html of the embedded video.
 */
function emvideo_ku6_preview($embed, $width, $height, $field, $item, &$node, $autoplay) {
  $output = theme('emvideo_ku6_flash', $item, $width, $height, $autoplay);
  return $output;
}

/**
 *  Implementation of hook_emfield_subtheme.
 *  This returns any theme functions defined by this provider.
 */
function emvideo_ku6_emfield_subtheme() {
    $themes = array(
        'emvideo_ku6_flash'  => array(
            'arguments' => array('item' => NULL, 'width' => NULL, 'height' => NULL, 'autoplay' => NULL),
            'file' => 'providers/ku6.inc',
            'path' => drupal_get_path('module', 'media_ku6'),
        )
    );
    return $themes;
}
