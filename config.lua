 
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
block.name = "light";
block.mainTexture = "textures/lightBlock.png";
block.lightValueR = 100;
block.lightValueG = 100;
block.lightValueB = 100;
block:registerBlock();

print "Configure finish"