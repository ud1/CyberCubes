 
print "Configure start"

block = Block:new();
block.id = 1;
block.name = "stone";
block.mainTexture = "textures/stone.png";
block:registerBlock();

block = Block:new();
block.id = 2;
block.name = "dirt";
block.mainTexture = "textures/dirt.png";
block:registerBlock();

block = Block:new();
block.id = 3;
block.name = "cobblestone";
block.mainTexture = "textures/cobblestone.png";
block:registerBlock();

block = Block:new();
block.id = 4;
block.name = "brick";
block.mainTexture = "textures/brick.png";
block:registerBlock();

block = Block:new();
block.id = 5;
block.name = "gravel";
block.mainTexture = "textures/gravel.png";
block:registerBlock();

block = Block:new();
block.id = 6;
block.name = "stonebrick";
block.mainTexture = "textures/stonebrick.png";
block:registerBlock();

block = Block:new();
block.id = 7;
block.name = "stone_slab_top";
block.mainTexture = "textures/stone_slab_top.png";
block:registerBlock();


block = Block:new();
block.id = 8;
block.name = "light";
block.mainTexture = "textures/lightBlock.png";
block.lightValueR = 100;
block.lightValueG = 100;
block.lightValueB = 100;
block:registerBlock();

block = Block:new();
block.id = 9;
block.name = "lightRed";
block.mainTexture = "textures/lightBlock.png";
block.lightValueR = 100;
block.lightValueG = 0;
block.lightValueB = 0;
block:registerBlock();

block = Block:new();
block.id = 10;
block.name = "lightGreen";
block.mainTexture = "textures/lightBlock.png";
block.lightValueR = 0;
block.lightValueG = 100;
block.lightValueB = 0;
block:registerBlock();

block = Block:new();
block.id = 11;
block.name = "lightBlue";
block.mainTexture = "textures/lightBlock.png";
block.lightValueR = 0;
block.lightValueG = 0;
block.lightValueB = 100;
block:registerBlock();


block = Block:new();
block.id = 20;
block.name = "bedrock";
block.mainTexture = "textures/bedrock.png";
block:registerBlock();

block = Block:new();
block.id = 21;
block.name = "sand";
block.mainTexture = "textures/sand.png";
block:registerBlock();

block = Block:new();
block.id = 22;
block.name = "gold_ore";
block.mainTexture = "textures/gold_ore.png";
block:registerBlock();

block = Block:new();
block.id = 23;
block.name = "iron_ore";
block.mainTexture = "textures/iron_ore.png";
block:registerBlock();

block = Block:new();
block.id = 24;
block.name = "coal_ore";
block.mainTexture = "textures/coal_ore.png";
block:registerBlock();

block = Block:new();
block.id = 25;
block.name = "lapis_ore";
block.mainTexture = "textures/lapis_ore.png";
block:registerBlock();

block = Block:new();
block.id = 26;
block.name = "sandstone";
block.mainTexture = "textures/sandstone_normal.png";
block.textureZN = "textures/sandstone_bottom.png";
block.textureZP = "textures/sandstone_top.png";
block:registerBlock();

block = Block:new();
block.id = 27;
block.name = "smooth_sandstone";
block.mainTexture = "textures/sandstone_smooth.png";
block.textureZN = "textures/sandstone_top.png";
block.textureZP = "textures/sandstone_top.png";
block:registerBlock();

block = Block:new();
block.id = 28;
block.name = "chiseled_sandstone";
block.mainTexture = "textures/sandstone_carved.png";
block.textureZN = "textures/sandstone_top.png";
block.textureZP = "textures/sandstone_top.png";
block:registerBlock();


block = Block:new();
block.id = 29;
block.name = "red_sand";
block.mainTexture = "textures/red_sand.png";
block:registerBlock();

block = Block:new();
block.id = 30;
block.name = "red_sandstone";
block.mainTexture = "textures/red_sandstone_normal.png";
block.textureZN = "textures/red_sandstone_bottom.png";
block.textureZP = "textures/red_sandstone_top.png";
block:registerBlock();

block = Block:new();
block.id = 31;
block.name = "smooth_red_sandstone";
block.mainTexture = "textures/red_sandstone_smooth.png";
block.textureZN = "textures/red_sandstone_top.png";
block.textureZP = "textures/red_sandstone_top.png";
block:registerBlock();

block = Block:new();
block.id = 32;
block.name = "chiseled_red_sandstone";
block.mainTexture = "textures/red_sandstone_carved.png";
block.textureZN = "textures/red_sandstone_top.png";
block.textureZP = "textures/red_sandstone_top.png";
block:registerBlock();

block = Block:new();
block.id = 33;
block.name = "diamond_ore";
block.mainTexture = "textures/diamond_ore.png";
block:registerBlock();

block = Block:new();
block.id = 34;
block.name = "redstone_ore";
block.mainTexture = "textures/redstone_ore.png";
block:registerBlock();

block = Block:new();
block.id = 35;
block.name = "snow";
block.mainTexture = "textures/snow.png";
block:registerBlock();

block = Block:new();
block.id = 36;
block.name = "clay";
block.mainTexture = "textures/clay.png";
block:registerBlock();

block = Block:new();
block.id = 37;
block.name = "mycelium";
block.mainTexture = "textures/mycelium_side.png";
block.textureZN = "textures/dirt.png";
block.textureZP = "textures/mycelium_top.png";
block:registerBlock();

block = Block:new();
block.id = 38;
block.name = "emerald_ore";
block.mainTexture = "textures/emerald_ore.png";
block:registerBlock();

block = Block:new();
block.id = 39;
block.name = "hardened_clay";
block.mainTexture = "textures/hardened_clay.png";
block:registerBlock();

block = Block:new();
block.id = 40;
block.name = "obsidian";
block.mainTexture = "textures/obsidian.png";
block:registerBlock();

block = Block:new();
block.id = 41;
block.name = "gold_block";
block.mainTexture = "textures/gold_block.png";
block:registerBlock();

block = Block:new();
block.id = 42;
block.name = "diamond_block";
block.mainTexture = "textures/diamond_block.png";
block:registerBlock();

block = Block:new();
block.id = 43;
block.name = "emerald_block";
block.mainTexture = "textures/emerald_block.png";
block:registerBlock();

block = Block:new();
block.id = 44;
block.name = "coal_block";
block.mainTexture = "textures/coal_block.png";
block:registerBlock();

block = Block:new();
block.id = 45;
block.name = "iron_block";
block.mainTexture = "textures/iron_block.png";
block:registerBlock();

block = Block:new();
block.id = 46;
block.name = "lapis_block";
block.mainTexture = "textures/lapis_block.png";
block:registerBlock();

block = Block:new();
block.id = 47;
block.name = "redstone_block";
block.mainTexture = "textures/redstone_block.png";
block:registerBlock();


block = Block:new();
block.id = 50;
block.name = "melon";
block.mainTexture = "textures/melon_side.png";
block.textureZN = "textures/melon_top.png";
block.textureZP = "textures/melon_top.png";
block:registerBlock();

block = Block:new();
block.id = 1000;
block.name = "test";
block.textureXN = "textures/XN.png";
block.textureXP = "textures/XP.png";
block.textureYN = "textures/YN.png";
block.textureYP = "textures/YP.png";
block.textureZN = "textures/ZN.png";
block.textureZP = "textures/ZP.png";
block.rotationGroup = 48;
block:registerBlock();


block = Block:new();
block.id = 30000;
block.name = "glass";
block.mainTexture = "textures/glass.png";
block:registerBlock();

block = Block:new();
block.id = 30001;
block.name = "glass_black";
block.mainTexture = "textures/glass_black.png";
block:registerBlock();

block = Block:new();
block.id = 30002;
block.name = "glass_blue";
block.mainTexture = "textures/glass_blue.png";
block:registerBlock();

block = Block:new();
block.id = 30003;
block.name = "glass_brown";
block.mainTexture = "textures/glass_brown.png";
block:registerBlock();

block = Block:new();
block.id = 30004;
block.name = "glass_cyan";
block.mainTexture = "textures/glass_cyan.png";
block:registerBlock();

block = Block:new();
block.id = 30005;
block.name = "glass_gray";
block.mainTexture = "textures/glass_gray.png";
block:registerBlock();

block = Block:new();
block.id = 30006;
block.name = "glass_green";
block.mainTexture = "textures/glass_green.png";
block:registerBlock();

block = Block:new();
block.id = 30007;
block.name = "glass_light_blue";
block.mainTexture = "textures/glass_light_blue.png";
block:registerBlock();

block = Block:new();
block.id = 30008;
block.name = "glass_lime";
block.mainTexture = "textures/glass_lime.png";
block:registerBlock();

block = Block:new();
block.id = 30009;
block.name = "glass_magenta";
block.mainTexture = "textures/glass_magenta.png";
block:registerBlock();

block = Block:new();
block.id = 30010;
block.name = "glass_orange";
block.mainTexture = "textures/glass_orange.png";
block:registerBlock();


block = Block:new();
block.id = 30011;
block.name = "glass_orig";
block.mainTexture = "textures/glass_orig.png";
block:registerBlock();

block = Block:new();
block.id = 30012;
block.name = "glass_black_orig";
block.mainTexture = "textures/glass_black_orig.png";
block:registerBlock();

block = Block:new();
block.id = 30013;
block.name = "glass_blue_orig";
block.mainTexture = "textures/glass_blue_orig.png";
block:registerBlock();

block = Block:new();
block.id = 30014;
block.name = "glass_brown_orig";
block.mainTexture = "textures/glass_brown_orig.png";
block:registerBlock();

block = Block:new();
block.id = 30015;
block.name = "glass_cyan_orig";
block.mainTexture = "textures/glass_cyan_orig.png";
block:registerBlock();

block = Block:new();
block.id = 30016;
block.name = "glass_gray_orig";
block.mainTexture = "textures/glass_gray_orig.png";
block:registerBlock();

block = Block:new();
block.id = 30017;
block.name = "glass_green_orig";
block.mainTexture = "textures/glass_green_orig.png";
block:registerBlock();

block = Block:new();
block.id = 30018;
block.name = "glass_light_blue_orig";
block.mainTexture = "textures/glass_light_blue_orig.png";
block:registerBlock();

block = Block:new();
block.id = 30019;
block.name = "glass_lime_orig";
block.mainTexture = "textures/glass_lime_orig.png";
block:registerBlock();

block = Block:new();
block.id = 30020;
block.name = "glass_magenta_orig";
block.mainTexture = "textures/glass_magenta_orig.png";
block:registerBlock();

block = Block:new();
block.id = 30021;
block.name = "glass_orange_orig";
block.mainTexture = "textures/glass_orange_orig.png";
block:registerBlock();

--------------------------------------------- Model blocks ---------------------------------------------
block = Block:new();
block.id = 35054;
block.name = "chest";
block.rotationGroup = 4;
block:registerBlock();

print "Configure finish"
