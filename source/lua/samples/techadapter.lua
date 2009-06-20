-- This scripts adds two TechAdapter to all players on the map
--
-- For more information about the ASC research system, please refer to
-- http://terdon.asc-hq.org/knowledgebase/show.php?ID=66
-- 
-- Available in: mapeditor



map = asc.getActiveMap()

for p = 0, 7 do
   map:getPlayer(p):getResearch():addPredefinedTechAdapter('o181')
   map:getPlayer(p):getResearch():addPredefinedTechAdapter('o2106')
end