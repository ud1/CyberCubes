 
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
block.id = 30000;
block.name = "glass_orange";
block.mainTexture = "textures/glass_orange.png";
block:registerBlock();

block = Block:new();
block.id = 30001;
block.name = "glass_light_blue";
block.mainTexture = "textures/glass_light_blue.png";
block:registerBlock();

print "Configure finish"