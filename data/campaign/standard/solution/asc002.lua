-- get handle to active map 
map = asc.getActiveMap() 

--Move unit WEASEL to #coord(3/7)#
r = asc.unitMovement ( map, 563, asc.MapCoordinate( 3, 7), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 1') 
  return
end 

--Move unit SHOCK TROOPER to #coord(3/7)#
r = asc.unitMovement ( map, 217, asc.MapCoordinate( 3, 7), -1 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 2') 
  return
end 

--Move unit WEASEL to #coord(6/10)#
r = asc.unitMovement ( map, 563, asc.MapCoordinate( 6, 10), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 3') 
  return
end 

--Move unit SHOCK TROOPER to #coord(3/9)#
r = asc.unitMovement ( map, 218, asc.MapCoordinate( 3, 9), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 4') 
  return
end 

--Move unit TERMITE to #coord(5/13)#
r = asc.unitMovement ( map, 221, asc.MapCoordinate( 5, 13), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 5') 
  return
end 

--Move unit TERMITE to #coord(5/11)#
r = asc.unitMovement ( map, 219, asc.MapCoordinate( 5, 11), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 6') 
  return
end 
-- Ending turn 1
asc.endTurn()

--Move unit SHOCK TROOPER to #coord(5/12)#
r = asc.unitMovement ( map, 218, asc.MapCoordinate( 5, 12), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 7') 
  return
end 

--Move unit WEASEL to #coord(9/3)#
r = asc.unitMovement ( map, 563, asc.MapCoordinate( 9, 3), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 8') 
  return
end 

--Move unit SHOCK TROOPER to #coord(9/2)#
r = asc.unitMovement ( map, 217, asc.MapCoordinate( 9, 2), -1 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 9') 
  return
end 

--Move unit TERMITE to #coord(7/15)#
r = asc.unitMovement ( map, 219, asc.MapCoordinate( 7, 15), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 10') 
  return
end 

--Move unit TERMITE to #coord(8/16)#
r = asc.unitMovement ( map, 220, asc.MapCoordinate( 8, 16), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 11') 
  return
end 

--Move unit ASSAULT TROOPER to #coord(8/18)#
r = asc.unitMovement ( map, 579, asc.MapCoordinate( 8, 18), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 12') 
  return
end 

--Move unit ASSAULT TROOPER to #coord(8/14)#
r = asc.unitMovement ( map, 580, asc.MapCoordinate( 8, 14), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 13') 
  return
end 

--Move unit ASSAULT TROOPER to #coord(6/13)#
r = asc.unitMovement ( map, 970, asc.MapCoordinate( 6, 13), -1 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 14') 
  return
end 

--Move unit WEASEL to #coord(7/14)#
r = asc.unitMovement ( map, 225, asc.MapCoordinate( 7, 14), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 15') 
  return
end 

--Move unit TERMITE to #coord(8/17)#
r = asc.unitMovement ( map, 221, asc.MapCoordinate( 8, 17), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 16') 
  return
end 

--Move unit SHOCK TROOPER to #coord(5/13)#
r = asc.unitMovement ( map, 218, asc.MapCoordinate( 5, 13), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 17') 
  return
end 
-- Ending turn 2
asc.endTurn()

--Move unit ASSAULT TROOPER to #coord(8/15)#
r = asc.unitMovement ( map, 580, asc.MapCoordinate( 8, 15), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 18') 
  return
end 

--Move unit TERMITE to #coord(6/13)#
r = asc.unitMovement ( map, 221, asc.MapCoordinate( 6, 13), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 19') 
  return
end 
-- Ending turn 3
asc.endTurn()

--Attacking  #coord(8/17)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 579, asc.MapCoordinate(8, 17), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 20') 
  return
end 

--Attacking  #coord(8/17)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 580, asc.MapCoordinate(8, 17), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 21') 
  return
end 

--Attacking  #coord(8/17)# with TERMITE
r = asc.unitAttack ( map, 220, asc.MapCoordinate(8, 17), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 22') 
  return
end 
-- Ending turn 4
asc.endTurn()

--Attacking  #coord(8/17)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 580, asc.MapCoordinate(8, 17), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 23') 
  return
end 

--Move unit TERMITE to #coord(10/20)#
r = asc.unitMovement ( map, 220, asc.MapCoordinate( 10, 20), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 24') 
  return
end 

--Move unit TERMITE to #coord(11/22)#
r = asc.unitMovement ( map, 219, asc.MapCoordinate( 11, 22), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 25') 
  return
end 

--Move unit ASSAULT TROOPER to #coord(9/19)#
r = asc.unitMovement ( map, 579, asc.MapCoordinate( 9, 19), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 26') 
  return
end 

--Move unit WEASEL to #coord(10/22)#
r = asc.unitMovement ( map, 225, asc.MapCoordinate( 10, 22), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 27') 
  return
end 

--Move unit TERMITE to #coord(9/21)#
r = asc.unitMovement ( map, 221, asc.MapCoordinate( 9, 21), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 28') 
  return
end 

--Move unit WEASEL to #coord(9/18)#
r = asc.unitMovement ( map, 224, asc.MapCoordinate( 9, 18), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 29') 
  return
end 

--Move unit SHOCK TROOPER to #coord(7/15)#
r = asc.unitMovement ( map, 218, asc.MapCoordinate( 7, 15), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 30') 
  return
end 
-- Ending turn 5
asc.endTurn()

--Move unit WEASEL to #coord(9/23)#
r = asc.unitMovement ( map, 225, asc.MapCoordinate( 9, 23), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 31') 
  return
end 

--Move unit TERMITE to #coord(10/23)#
r = asc.unitMovement ( map, 221, asc.MapCoordinate( 10, 23), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 32') 
  return
end 

--Move unit ASSAULT TROOPER to #coord(10/22)#
r = asc.unitMovement ( map, 579, asc.MapCoordinate( 10, 22), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 33') 
  return
end 

--Attacking  #coord(10/21)# with TERMITE
r = asc.unitAttack ( map, 219, asc.MapCoordinate(10, 21), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 34') 
  return
end 

--Attacking  #coord(10/21)# with TERMITE
r = asc.unitAttack ( map, 221, asc.MapCoordinate(10, 21), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 35') 
  return
end 

--Attacking  #coord(10/21)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 579, asc.MapCoordinate(10, 21), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 36') 
  return
end 

--Attacking  #coord(10/21)# with TERMITE
r = asc.unitAttack ( map, 220, asc.MapCoordinate(10, 21), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 37') 
  return
end 

--Move unit WEASEL to #coord(9/19)#
r = asc.unitMovement ( map, 224, asc.MapCoordinate( 9, 19), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 38') 
  return
end 

--Attacking  #coord(10/18)# with WEASEL
r = asc.unitAttack ( map, 224, asc.MapCoordinate(10, 18), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 39') 
  return
end 
-- Ending turn 6
asc.endTurn()

--Move unit TERMITE to #coord(12/22)#
r = asc.unitMovement ( map, 221, asc.MapCoordinate( 12, 22), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 40') 
  return
end 

--Move unit TERMITE to #coord(12/20)#
r = asc.unitMovement ( map, 219, asc.MapCoordinate( 12, 20), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 41') 
  return
end 

--Move unit WEASEL to #coord(7/14)#
r = asc.unitMovement ( map, 563, asc.MapCoordinate( 7, 14), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 42') 
  return
end 

--Move unit WEASEL to #coord(10/19)#
r = asc.unitMovement ( map, 224, asc.MapCoordinate( 10, 19), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 43') 
  return
end 

--Move unit ASSAULT TROOPER to #coord(9/19)#
r = asc.unitMovement ( map, 580, asc.MapCoordinate( 9, 19), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 44') 
  return
end 

--Attacking  #coord(10/18)# with WEASEL
r = asc.unitAttack ( map, 224, asc.MapCoordinate(10, 18), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 45') 
  return
end 

--Move unit WEASEL to #coord(11/20)#
r = asc.unitMovement ( map, 225, asc.MapCoordinate( 11, 20), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 46') 
  return
end 

--Attacking  #coord(10/21)# with WEASEL
r = asc.unitAttack ( map, 225, asc.MapCoordinate(10, 21), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 47') 
  return
end 

--Attacking  #coord(10/21)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 579, asc.MapCoordinate(10, 21), 1 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 48') 
  return
end 

--Attacking  #coord(11/21)# with TERMITE
r = asc.unitAttack ( map, 219, asc.MapCoordinate(11, 21), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 49') 
  return
end 

--Attacking  #coord(11/21)# with TERMITE
r = asc.unitAttack ( map, 221, asc.MapCoordinate(11, 21), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 50') 
  return
end 
-- Ending turn 7
asc.endTurn()

--Move unit ASSAULT TROOPER to #coord(11/22)#
r = asc.unitMovement ( map, 579, asc.MapCoordinate( 11, 22), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 51') 
  return
end 

--Attacking  #coord(11/21)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 579, asc.MapCoordinate(11, 21), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 52') 
  return
end 

--Move unit WEASEL to #coord(11/19)#
r = asc.unitMovement ( map, 225, asc.MapCoordinate( 11, 19), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 53') 
  return
end 

--Move unit ASSAULT TROOPER to #coord(11/20)#
r = asc.unitMovement ( map, 580, asc.MapCoordinate( 11, 20), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 54') 
  return
end 

--Attacking  #coord(11/21)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 580, asc.MapCoordinate(11, 21), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 55') 
  return
end 

--Move unit WEASEL to #coord(11/15)#
r = asc.unitMovement ( map, 224, asc.MapCoordinate( 11, 15), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 56') 
  return
end 

--Attacking  #coord(11/17)# with WEASEL
r = asc.unitAttack ( map, 224, asc.MapCoordinate(11, 17), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 57') 
  return
end 

--Move unit TERMITE to #coord(12/17)#
r = asc.unitMovement ( map, 221, asc.MapCoordinate( 12, 17), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 58') 
  return
end 

--Attacking  #coord(12/18)# with TERMITE
r = asc.unitAttack ( map, 221, asc.MapCoordinate(12, 18), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 59') 
  return
end 

--Attacking  #coord(11/17)# with WEASEL
r = asc.unitAttack ( map, 225, asc.MapCoordinate(11, 17), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 60') 
  return
end 

--Move unit WEASEL to #coord(10/21)#
r = asc.unitMovement ( map, 563, asc.MapCoordinate( 10, 21), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 61') 
  return
end 

--Move unit SHOCK TROOPER to #coord(9/19)#
r = asc.unitMovement ( map, 218, asc.MapCoordinate( 9, 19), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 62') 
  return
end 
-- Ending turn 8
asc.endTurn()

--Move unit ASSAULT TROOPER to #coord(12/16)#
r = asc.unitMovement ( map, 580, asc.MapCoordinate( 12, 16), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 63') 
  return
end 

--Attacking  #coord(12/14)# with ASSAULT TROOPER
r = asc.unitAttack ( map, 580, asc.MapCoordinate(12, 14), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 64') 
  return
end 

--Move unit WEASEL to #coord(12/13)#
r = asc.unitMovement ( map, 224, asc.MapCoordinate( 12, 13), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 65') 
  return
end 

--Attacking  #coord(12/15)# with TERMITE
r = asc.unitAttack ( map, 221, asc.MapCoordinate(12, 15), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 66') 
  return
end 

--Move unit WEASEL to #coord(14/20)#
r = asc.unitMovement ( map, 563, asc.MapCoordinate( 14, 20), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 67') 
  return
end 

--Move unit WEASEL to #coord(14/21)#
r = asc.unitMovement ( map, 225, asc.MapCoordinate( 14, 21), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 68') 
  return
end 

--Move unit ASSAULT TROOPER to #coord(12/21)#
r = asc.unitMovement ( map, 579, asc.MapCoordinate( 12, 21), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 69') 
  return
end 

--Move unit SHOCK TROOPER to #coord(11/19)#
r = asc.unitMovement ( map, 218, asc.MapCoordinate( 11, 19), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 70') 
  return
end 

--Move unit WEASEL to #coord(13/14)#
r = asc.unitMovement ( map, 224, asc.MapCoordinate( 13, 14), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 71') 
  return
end 
-- Ending turn 9
asc.endTurn()

--Move unit WEASEL to #coord(16/18)#
r = asc.unitMovement ( map, 563, asc.MapCoordinate( 16, 18), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 72') 
  return
end 

--Move unit WEASEL to #coord(16/17)#
r = asc.unitMovement ( map, 563, asc.MapCoordinate( 16, 17), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 73') 
  return
end 

--Move unit WEASEL to #coord(17/17)#
r = asc.unitMovement ( map, 563, asc.MapCoordinate( 17, 17), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 74') 
  return
end 

--Move unit WEASEL to #coord(17/22)#
r = asc.unitMovement ( map, 225, asc.MapCoordinate( 17, 22), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 75') 
  return
end 

--Move unit WEASEL to #coord(17/22)#
r = asc.unitMovement ( map, 225, asc.MapCoordinate( 17, 22), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 76') 
  return
end 

--Move unit WEASEL to #coord(17/24)#
r = asc.unitMovement ( map, 225, asc.MapCoordinate( 17, 24), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 77') 
  return
end 

--Move unit ASSAULT TROOPER to #coord(18/24)#
r = asc.unitMovement ( map, 970, asc.MapCoordinate( 18, 24), -1 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 78') 
  return
end 
-- Ending turn 10
asc.endTurn()
-- Ending turn 11
asc.endTurn()
-- Ending turn 12
asc.endTurn()
-- Ending turn 13
asc.endTurn()
-- Ending turn 14
asc.endTurn()

--Move unit ASSAULT TROOPER to #coord(13/14)#
r = asc.unitMovement ( map, 580, asc.MapCoordinate( 13, 14), -1 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 79') 
  return
end 

--Move unit WEASEL to #coord(15/18)#
r = asc.unitMovement ( map, 224, asc.MapCoordinate( 15, 18), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 80') 
  return
end 

--Move unit SHOCK TROOPER to #coord(13/21)#
r = asc.unitMovement ( map, 218, asc.MapCoordinate( 13, 21), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 81') 
  return
end 

--Move unit ASSAULT TROOPER to #coord(19/20)#
r = asc.unitMovement ( map, 970, asc.MapCoordinate( 19, 20), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 82') 
  return
end 
-- Ending turn 15
asc.endTurn()

--Move unit WEASEL to #coord(16/17)#
r = asc.unitMovement ( map, 224, asc.MapCoordinate( 16, 17), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 83') 
  return
end 

--Move unit WEASEL to #coord(16/17)#
r = asc.unitMovement ( map, 224, asc.MapCoordinate( 16, 17), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 84') 
  return
end 

--Move unit ASSAULT TROOPER to #coord(17/15)#
r = asc.unitMovement ( map, 580, asc.MapCoordinate( 17, 15), -1 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 85') 
  return
end 

--Move unit ASSAULT TROOPER to #coord(19/16)#
r = asc.unitMovement ( map, 970, asc.MapCoordinate( 19, 16), -1 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 86') 
  return
end 
