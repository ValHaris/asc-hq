-- This is an example about how to create a Tutorial with Lua.
-- It loads a savegame (which you need to copy into your ASC directory)
-- and runs units there.
--
-- The tutorial was created by:
--   - doing the actions of the first variant in ASC
--   - saving them as Lua script,
--   - playing the second variant
--   - saving again
--   - merging the resulting files in a text editor and adding the infoMessages
--
-- Available in: ASC



map = asc.loadGame( 'turorial-lua-demo1.sav' )

asc.infoMessage("This tutorial demonstrates the tactic called 'hemming'")

asc.infoMessage("At first, we attack with all units sequentially.")


--Move unit UnitTest ASSAULT to #coord(5/9)#
r = asc.unitMovement ( map, 205, asc.MapCoordinate( 5, 9), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Attacking  #coord(6/10)# with UnitTest ASSAULT
r = asc.unitAttack ( map, 205, asc.MapCoordinate(6, 10), 0 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit ASSAULT TROOPER to #coord(6/8)#
r = asc.unitMovement ( map, 207, asc.MapCoordinate( 6, 8), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Attacking  #coord(6/10)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 207, asc.MapCoordinate(6, 10), 0 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit ASSAULT TROOPER to #coord(6/9)#
r = asc.unitMovement ( map, 210, asc.MapCoordinate( 6, 9), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Attacking  #coord(6/10)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 210, asc.MapCoordinate(6, 10), 0 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit UnitTest ASSAULT to #coord(5/11)#
r = asc.unitMovement ( map, 204, asc.MapCoordinate( 5, 11), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Attacking  #coord(6/10)# with UnitTest ASSAULT
r = asc.unitAttack ( map, 204, asc.MapCoordinate(6, 10), 0 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit ASSAULT TROOPER to #coord(6/12)#
r = asc.unitMovement ( map, 208, asc.MapCoordinate( 6, 12), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Attacking  #coord(6/10)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 208, asc.MapCoordinate(6, 10), 0 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit ASSAULT TROOPER to #coord(6/11)#
r = asc.unitMovement ( map, 209, asc.MapCoordinate( 6, 11), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Attacking  #coord(6/10)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 209, asc.MapCoordinate(6, 10), 0 )
if r:successful()==false then asc.displayActionError(r) end 


asc.infoMessage("The enemy tank has survived")


map = asc.loadGame( 'turorial-lua-demo1.sav' )


asc.infoMessage("Now are first surrounding the tank with our units and *then* attack ")

--Move unit UnitTest ASSAULT to #coord(5/9)#
r = asc.unitMovement ( map, 205, asc.MapCoordinate( 5, 9), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit UnitTest ASSAULT to #coord(5/11)#
r = asc.unitMovement ( map, 204, asc.MapCoordinate( 5, 11), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit ASSAULT TROOPER to #coord(6/12)#
r = asc.unitMovement ( map, 208, asc.MapCoordinate( 6, 12), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit ASSAULT TROOPER to #coord(6/11)#
r = asc.unitMovement ( map, 209, asc.MapCoordinate( 6, 11), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit ASSAULT TROOPER to #coord(6/8)#
r = asc.unitMovement ( map, 207, asc.MapCoordinate( 6, 8), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit ASSAULT TROOPER to #coord(6/9)#
r = asc.unitMovement ( map, 210, asc.MapCoordinate( 6, 9), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Attacking  #coord(6/10)# with UnitTest ASSAULT
r = asc.unitAttack ( map, 205, asc.MapCoordinate(6, 10), 0 )
if r:successful()==false then asc.displayActionError(r) end 

--Attacking  #coord(6/10)# with UnitTest ASSAULT
r = asc.unitAttack ( map, 204, asc.MapCoordinate(6, 10), 0 )
if r:successful()==false then asc.displayActionError(r) end 

--Attacking  #coord(6/10)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 207, asc.MapCoordinate(6, 10), 0 )
if r:successful()==false then asc.displayActionError(r) end 

asc.infoMessage("After 3 attacks the enemy tank is already destroyed!")
