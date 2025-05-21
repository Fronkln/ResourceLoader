# ResourceLoader
Load Yakuza resources at will when the player starts

## Supported
Yakuza 5 - GMT

## Requirements
Latest version of Shin Ryu Mod Manager

## Usage (for Modders)
1) Ensure ResourceLoader is installed from Libraries tab in Shin Ryu Mod Manager
2) Add `Dependencies: 0e29b1f2-415a-49f7-8a6c-bae52d75b5dc;` to your mod-meta.yaml
3) Create a file with the name `player_spawn_load_resource_list.txt` in your mod folder root.
4) Add list of resources you want to load when the player spawns inside the file<br>
Example:<br>
p_ich_btl_sud_stda_st.gmt<br>
p_ich_btl_sud_stda_lp.gmt<br>
p_ich_btl_sud_stda_ed.gmt<br>
p_ich_btl_sud_ath_01.gmt<br>
p_ich_btl_sud_ath_02.gmt<br>
p_ich_btl_sud_ath_03.gmt<br>
p_ich_btl_sud_ath_04.gmt<br>
