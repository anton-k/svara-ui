#pragma once
#include <string>
#include <map>
#include <Icons.h>
#include <plog/Log.h>

using Icon = std::pair<const char*, int>;

std::map<std::string, Icon> iconDataMap{
  { "adr", { Icons::fadADR_svg, Icons::fadADR_svgSize }},
  { "adsr", { Icons::fadADSR_svg, Icons::fadADSR_svgSize }},
  { "ahdsr", { Icons::fadAHDSR_svg, Icons::fadAHDSR_svgSize }} ,
  { "armrecording", {Icons::fadarmrecording_svg , Icons::fadarmrecording_svgSize  }},
  { "arpchord", {Icons::fadarpchord_svg , Icons::fadarpchord_svgSize  }},
  { "arpdownandup", {Icons::fadarpdownandup_svg , Icons::fadarpdownandup_svgSize  }},
  { "arpdown", {Icons::fadarpdown_svg , Icons::fadarpdown_svgSize  }},
  { "arpdownup", {Icons::fadarpdownup_svg , Icons::fadarpdownup_svgSize  }},
  { "arpplayorder", {Icons::fadarpplayorder_svg , Icons::fadarpplayorder_svgSize  }},
  { "arprandom", {Icons::fadarprandom_svg , Icons::fadarprandom_svgSize  }},
  { "arpupandown", {Icons::fadarpupandown_svg , Icons::fadarpupandown_svgSize  }},
  { "arpupdown", {Icons::fadarpupdown_svg , Icons::fadarpupdown_svgSize  }},
  { "arpup", {Icons::fadarpup_svg , Icons::fadarpup_svgSize  }},
  { "arrowshorz", {Icons::fadarrowshorz_svg , Icons::fadarrowshorz_svgSize  }},
  { "arrowsvert", {Icons::fadarrowsvert_svg , Icons::fadarrowsvert_svgSize  }},
  { "ar", {Icons::fadAR_svg, Icons::fadAR_svgSize  }},
  { "automation2p", {Icons::fadautomation2p_svg , Icons::fadautomation2p_svgSize  }},
  { "automation3p", {Icons::fadautomation3p_svg , Icons::fadautomation3p_svgSize  }},
  { "automation4p", {Icons::fadautomation4p_svg , Icons::fadautomation4p_svgSize  }},
  { "backward", {Icons::fadbackward_svg , Icons::fadbackward_svgSize  }},
  { "bluetooth", {Icons::fadbluetooth_svg , Icons::fadbluetooth_svgSize  }},
  { "caretdown", {Icons::fadcaretdown_svg , Icons::fadcaretdown_svgSize  }},
  { "caretleft", {Icons::fadcaretleft_svg , Icons::fadcaretleft_svgSize  }},
  { "caretright", {Icons::fadcaretright_svg , Icons::fadcaretright_svgSize  }},
  { "caretup", {Icons::fadcaretup_svg , Icons::fadcaretup_svgSize  }},
  { "close", {Icons::fadclose_svg , Icons::fadclose_svgSize  }},
  { "copy", {Icons::fadcopy_svg , Icons::fadcopy_svgSize  }},
  { "cpu", {Icons::fadcpu_svg , Icons::fadcpu_svgSize  }},
  { "cutter", {Icons::fadcutter_svg , Icons::fadcutter_svgSize  }},
  { "digital0", {Icons::faddigital0_svg , Icons::faddigital0_svgSize  }},
  { "digital1", {Icons::faddigital1_svg , Icons::faddigital1_svgSize  }},
  { "digital2", {Icons::faddigital2_svg , Icons::faddigital2_svgSize  }},
  { "digital3", {Icons::faddigital3_svg , Icons::faddigital3_svgSize  }},
  { "digital4", {Icons::faddigital4_svg , Icons::faddigital4_svgSize  }},
  { "digital5", {Icons::faddigital5_svg , Icons::faddigital5_svgSize  }},
  { "digital6", {Icons::faddigital6_svg , Icons::faddigital6_svgSize  }},
  { "digital7", {Icons::faddigital7_svg , Icons::faddigital7_svgSize  }},
  { "digital8", {Icons::faddigital8_svg , Icons::faddigital8_svgSize  }},
  { "digital9", {Icons::faddigital9_svg , Icons::faddigital9_svgSize  }},
  { "digitalcolon", {Icons::faddigitalcolon_svg , Icons::faddigitalcolon_svgSize  }},
  { "digitaldot", {Icons::faddigitaldot_svg , Icons::faddigitaldot_svgSize  }},
  { "diskio", {Icons::faddiskio_svg , Icons::faddiskio_svgSize  }},
  { "drumpad", {Icons::faddrumpad_svg , Icons::faddrumpad_svgSize  }},
  { "duplicate", {Icons::fadduplicate_svg , Icons::fadduplicate_svgSize  }},
  { "eraser", {Icons::faderaser_svg , Icons::faderaser_svgSize  }},
  { "ffwd", {Icons::fadffwd_svg , Icons::fadffwd_svgSize  }},
  { "filterbandpass", {Icons::fadfilterbandpass_svg , Icons::fadfilterbandpass_svgSize  }},
  { "filterbell", {Icons::fadfilterbell_svg , Icons::fadfilterbell_svgSize  }},
  { "filterbypass", {Icons::fadfilterbypass_svg , Icons::fadfilterbypass_svgSize  }},
  { "filterhighpass", {Icons::fadfilterhighpass_svg , Icons::fadfilterhighpass_svgSize  }},
  { "filterlowpass", {Icons::fadfilterlowpass_svg , Icons::fadfilterlowpass_svgSize  }},
  { "filternotch", {Icons::fadfilternotch_svg , Icons::fadfilternotch_svgSize  }},
  { "filterrezhighpass", {Icons::fadfilterrezhighpass_svg , Icons::fadfilterrezhighpass_svgSize  }},
  { "filterrezlowpass", {Icons::fadfilterrezlowpass_svg , Icons::fadfilterrezlowpass_svgSize  }},
  { "filtershelvinghi", {Icons::fadfiltershelvinghi_svg , Icons::fadfiltershelvinghi_svgSize  }},
  { "filtershelvinglo", {Icons::fadfiltershelvinglo_svg , Icons::fadfiltershelvinglo_svgSize  }},
  { "foldback", {Icons::fadfoldback_svg , Icons::fadfoldback_svgSize  }},
  { "forward", {Icons::fadforward_svg , Icons::fadforward_svgSize  }},
  { "hardclipcurve", {Icons::fadhardclipcurve_svg , Icons::fadhardclipcurve_svgSize  }},
  { "hardclip", {Icons::fadhardclip_svg , Icons::fadhardclip_svgSize  }},
  { "headphones", {Icons::fadheadphones_svg , Icons::fadheadphones_svgSize  }},
  { "hexpand", {Icons::fadhexpand_svg , Icons::fadhexpand_svgSize  }},
  { "keyboard", {Icons::fadkeyboard_svg , Icons::fadkeyboard_svgSize  }},
  { "lock", {Icons::fadlock_svg , Icons::fadlock_svgSize  }},
  { "logoaax", {Icons::fadlogoaax_svg , Icons::fadlogoaax_svgSize  }},
  { "logoabletonlink", {Icons::fadlogoabletonlink_svg , Icons::fadlogoabletonlink_svgSize  }},
  { "logoaudacity", {Icons::fadlogoaudacity_svg , Icons::fadlogoaudacity_svgSize  }},
  { "logoaudiobus", {Icons::fadlogoaudiobus_svg , Icons::fadlogoaudiobus_svgSize  }},
  { "logoau", {Icons::fadlogoau_svg , Icons::fadlogoau_svgSize  }},
  { "logocubase", {Icons::fadlogocubase_svg , Icons::fadlogocubase_svgSize  }},
  { "logofl", {Icons::fadlogofl_svg , Icons::fadlogofl_svgSize  }},
  { "logojuce", {Icons::fadlogojuce_svg , Icons::fadlogojuce_svgSize  }},
  { "logoladspa", {Icons::fadlogoladspa_svg , Icons::fadlogoladspa_svgSize  }},
  { "logolive", {Icons::fadlogolive_svg , Icons::fadlogolive_svgSize  }},
  { "logolv2", {Icons::fadlogolv2_svg , Icons::fadlogolv2_svgSize  }},
  { "logoprotools", {Icons::fadlogoprotools_svg , Icons::fadlogoprotools_svgSize  }},
  { "logorackext", {Icons::fadlogorackext_svg , Icons::fadlogorackext_svgSize  }},
  { "logoreaper", {Icons::fadlogoreaper_svg , Icons::fadlogoreaper_svgSize  }},
  { "logoreason", {Icons::fadlogoreason_svg , Icons::fadlogoreason_svgSize  }},
  { "logorewire", {Icons::fadlogorewire_svg , Icons::fadlogorewire_svgSize  }},
  { "logostudioone", {Icons::fadlogostudioone_svg , Icons::fadlogostudioone_svgSize  }},
  { "logotracktion", {Icons::fadlogotracktion_svg , Icons::fadlogotracktion_svgSize  }},
  { "logovst", {Icons::fadlogovst_svg , Icons::fadlogovst_svgSize  }},
  { "logowaveform", {Icons::fadlogowaveform_svg , Icons::fadlogowaveform_svgSize  }},
  { "loop", {Icons::fadloop_svg , Icons::fadloop_svgSize  }},
  { "metronome", {Icons::fadmetronome_svg , Icons::fadmetronome_svgSize  }},
  { "microphone", {Icons::fadmicrophone_svg , Icons::fadmicrophone_svgSize  }},
  { "midiplug", {Icons::fadmidiplug_svg , Icons::fadmidiplug_svgSize  }},
  { "modrandom", {Icons::fadmodrandom_svg , Icons::fadmodrandom_svgSize  }},
  { "modsawdown", {Icons::fadmodsawdown_svg , Icons::fadmodsawdown_svgSize  }},
  { "modsawup", {Icons::fadmodsawup_svg , Icons::fadmodsawup_svgSize  }},
  { "modsh", {Icons::fadmodsh_svg , Icons::fadmodsh_svgSize  }},
  { "modsine", {Icons::fadmodsine_svg , Icons::fadmodsine_svgSize  }},
  { "modsquare", {Icons::fadmodsquare_svg , Icons::fadmodsquare_svgSize  }},
  { "modtri", {Icons::fadmodtri_svg , Icons::fadmodtri_svgSize  }},
  { "modularplug", {Icons::fadmodularplug_svg , Icons::fadmodularplug_svgSize  }},
  { "mono", {Icons::fadmono_svg , Icons::fadmono_svgSize  }},
  { "mute", {Icons::fadmute_svg , Icons::fadmute_svgSize  }},
  { "next", {Icons::fadnext_svg , Icons::fadnext_svgSize  }},
  { "open", {Icons::fadopen_svg , Icons::fadopen_svgSize  }},
  { "paste", {Icons::fadpaste_svg , Icons::fadpaste_svgSize  }},
  { "pause", {Icons::fadpause_svg , Icons::fadpause_svgSize  }},
  { "pen", {Icons::fadpen_svg , Icons::fadpen_svgSize  }},
  { "phase", {Icons::fadphase_svg , Icons::fadphase_svgSize  }},
  { "play", {Icons::fadplay_svg , Icons::fadplay_svgSize  }},
  { "pointer", {Icons::fadpointer_svg , Icons::fadpointer_svgSize  }},
  { "powerswitch", {Icons::fadpowerswitch_svg , Icons::fadpowerswitch_svgSize  }},
  { "presetab", {Icons::fadpresetab_svg , Icons::fadpresetab_svgSize  }},
  { "preseta", {Icons::fadpreseta_svg , Icons::fadpreseta_svgSize  }},
  { "presetba", {Icons::fadpresetba_svg , Icons::fadpresetba_svgSize  }},
  { "presetb", {Icons::fadpresetb_svg , Icons::fadpresetb_svgSize  }},
  { "prev", {Icons::fadprev_svg , Icons::fadprev_svgSize  }},
  { "punchin", {Icons::fadpunchin_svg , Icons::fadpunchin_svgSize  }},
  { "punchout", {Icons::fadpunchout_svg , Icons::fadpunchout_svgSize  }},
  { "ram", {Icons::fadram_svg , Icons::fadram_svgSize  }},
  { "random1dice", {Icons::fadrandom1dice_svg , Icons::fadrandom1dice_svgSize  }},
  { "random2dice", {Icons::fadrandom2dice_svg , Icons::fadrandom2dice_svgSize  }},
  { "record", {Icons::fadrecord_svg , Icons::fadrecord_svgSize  }},
  { "redo", {Icons::fadredo_svg , Icons::fadredo_svgSize  }},
  { "repeatone", {Icons::fadrepeatone_svg , Icons::fadrepeatone_svgSize  }},
  { "repeat", {Icons::fadrepeat_svg , Icons::fadrepeat_svgSize  }},
  { "rew", {Icons::fadrew_svg , Icons::fadrew_svgSize  }},
  { "roundswitchoff", {Icons::fadroundswitchoff_svg , Icons::fadroundswitchoff_svgSize  }},
  { "roundswitchon", {Icons::fadroundswitchon_svg , Icons::fadroundswitchon_svgSize  }},
  { "saveas", {Icons::fadsaveas_svg , Icons::fadsaveas_svgSize  }},
  { "save", {Icons::fadsave_svg , Icons::fadsave_svgSize  }},
  { "scissors", {Icons::fadscissors_svg , Icons::fadscissors_svgSize  }},
  { "shuffle", {Icons::fadshuffle_svg , Icons::fadshuffle_svgSize  }},
  { "sliderhandle1", {Icons::fadsliderhandle1_svg , Icons::fadsliderhandle1_svgSize  }},
  { "sliderhandle2", {Icons::fadsliderhandle2_svg , Icons::fadsliderhandle2_svgSize  }},
  { "sliderround1", {Icons::fadsliderround1_svg , Icons::fadsliderround1_svgSize  }},
  { "sliderround2", {Icons::fadsliderround2_svg , Icons::fadsliderround2_svgSize  }},
  { "sliderround3", {Icons::fadsliderround3_svg , Icons::fadsliderround3_svgSize  }},
  { "softclipcurve", {Icons::fadsoftclipcurve_svg , Icons::fadsoftclipcurve_svgSize  }},
  { "softclip", {Icons::fadsoftclip_svg , Icons::fadsoftclip_svgSize  }},
  { "solo", {Icons::fadsolo_svg , Icons::fadsolo_svgSize  }},
  { "speaker", {Icons::fadspeaker_svg , Icons::fadspeaker_svgSize  }},
  { "squareswitchoff", {Icons::fadsquareswitchoff_svg , Icons::fadsquareswitchoff_svgSize  }},
  { "squareswitchon", {Icons::fadsquareswitchon_svg , Icons::fadsquareswitchon_svgSize  }},
  { "stereo", {Icons::fadstereo_svg , Icons::fadstereo_svgSize  }},
  { "stop", {Icons::fadstop_svg , Icons::fadstop_svgSize  }},
  { "thunderbolt", {Icons::fadthunderbolt_svg , Icons::fadthunderbolt_svgSize  }},
  { "timeselect", {Icons::fadtimeselect_svg , Icons::fadtimeselect_svgSize  }},
  { "undo", {Icons::fadundo_svg , Icons::fadundo_svgSize  }},
  { "unlock", {Icons::fadunlock_svg , Icons::fadunlock_svgSize  }},
  { "usb", {Icons::fadusb_svg , Icons::fadusb_svgSize  }},
  { "vexpand", {Icons::fadvexpand_svg , Icons::fadvexpand_svgSize  }},
  { "vroundswitchoff", {Icons::fadvroundswitchoff_svg , Icons::fadvroundswitchoff_svgSize  }},
  { "vroundswitchon", {Icons::fadvroundswitchon_svg , Icons::fadvroundswitchon_svgSize  }},
  { "vsquareswitchoff", {Icons::fadvsquareswitchoff_svg , Icons::fadvsquareswitchoff_svgSize  }},
  { "vsquareswitchon", {Icons::fadvsquareswitchon_svg , Icons::fadvsquareswitchon_svgSize  }},
  { "waveform", {Icons::fadwaveform_svg , Icons::fadwaveform_svgSize  }},
  { "xlrplug", {Icons::fadxlrplug_svg , Icons::fadxlrplug_svgSize  }},
  { "zoomin", {Icons::fadzoomin_svg , Icons::fadzoomin_svgSize  }},
  { "zoomout", {Icons::fadzoomout_svg , Icons::fadzoomout_svgSize  }}
};

Icon  getIcon(std::string name) {
  if (iconDataMap.count(name) != 0) {
    return iconDataMap[name];
  } else {
    PLOG_ERROR << "No icon with name: " << name;
    return {Icons::fadlogojuce_svg, Icons::fadlogojuce_svgSize};
  }
}
