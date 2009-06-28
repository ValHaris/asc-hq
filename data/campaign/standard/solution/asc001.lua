-- get handle to active map 
map = asc.getActiveMap() 

--Move unit SHOCK TROOPER to #coord(5/3)#
r = asc.unitMovement ( map, 414, asc.MapCoordinate( 5, 3), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 1') 
  return
end 

--Move unit SHOCK TROOPER to #coord(4/5)#
r = asc.unitMovement ( map, 414, asc.MapCoordinate( 4, 5), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 2') 
  return
end 

--Move unit SHOCK TROOPER to #coord(2/7)#
r = asc.unitMovement ( map, 379, asc.MapCoordinate( 2, 7), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 3') 
  return
end 

--Move unit SHOCK TROOPER to #coord(3/8)#
r = asc.unitMovement ( map, 379, asc.MapCoordinate( 3, 8), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 4') 
  return
end 

--Move unit SHOCK TROOPER to #coord(2/8)#
r = asc.unitMovement ( map, 411, asc.MapCoordinate( 2, 8), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 5') 
  return
end 

--Move unit SHOCK TROOPER to #coord(1/11)#
r = asc.unitMovement ( map, 415, asc.MapCoordinate( 1, 11), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 6') 
  return
end 
-- Ending turn 1
asc.endTurn()

--Move unit SHOCK TROOPER to #coord(3/12)#
r = asc.unitMovement ( map, 415, asc.MapCoordinate( 3, 12), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 7') 
  return
end 

--Move unit SHOCK TROOPER to #coord(3/10)#
r = asc.unitMovement ( map, 411, asc.MapCoordinate( 3, 10), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 8') 
  return
end 

--Move unit SHOCK TROOPER to #coord(4/9)#
r = asc.unitMovement ( map, 414, asc.MapCoordinate( 4, 9), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 9') 
  return
end 

--Move unit SHOCK TROOPER to #coord(4/10)#
r = asc.unitMovement ( map, 379, asc.MapCoordinate( 4, 10), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 10') 
  return
end 

--Move unit SHOCK TROOPER to #coord(3/11)#
r = asc.unitMovement ( map, 411, asc.MapCoordinate( 3, 11), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 11') 
  return
end 
-- Ending turn 2
asc.endTurn()

--Move unit SHOCK TROOPER to #coord(3/15)#
r = asc.unitMovement ( map, 415, asc.MapCoordinate( 3, 15), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 12') 
  return
end 

--Attacking  #coord(3/13)# with SHOCK TROOPER
r = asc.unitAttack ( map, 415, asc.MapCoordinate(3, 13), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 13') 
  return
end 

--Move unit SHOCK TROOPER to #coord(4/14)#
r = asc.unitMovement ( map, 411, asc.MapCoordinate( 4, 14), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 14') 
  return
end 

--Move unit SHOCK TROOPER to #coord(3/11)#
r = asc.unitMovement ( map, 414, asc.MapCoordinate( 3, 11), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 15') 
  return
end 

--Attacking  #coord(4/12)# with SHOCK TROOPER
r = asc.unitAttack ( map, 414, asc.MapCoordinate(4, 12), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 16') 
  return
end 

--Attacking  #coord(4/12)# with SHOCK TROOPER
r = asc.unitAttack ( map, 411, asc.MapCoordinate(4, 12), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 17') 
  return
end 

--Move unit SHOCK TROOPER to #coord(5/10)#
r = asc.unitMovement ( map, 379, asc.MapCoordinate( 5, 10), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 18') 
  return
end 

--Attacking  #coord(4/11)# with SHOCK TROOPER
r = asc.unitAttack ( map, 379, asc.MapCoordinate(4, 11), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 19') 
  return
end 
-- Ending turn 3
asc.endTurn()

--Move unit SHOCK TROOPER to #coord(5/13)#
r = asc.unitMovement ( map, 379, asc.MapCoordinate( 5, 13), -1 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 20') 
  return
end 

--Move unit SHOCK TROOPER to #coord(4/15)#
r = asc.unitMovement ( map, 415, asc.MapCoordinate( 4, 15), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 21') 
  return
end 

--Attacking  #coord(4/16)# with SHOCK TROOPER
r = asc.unitAttack ( map, 415, asc.MapCoordinate(4, 16), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 22') 
  return
end 

--Move unit SHOCK TROOPER to #coord(5/15)#
r = asc.unitMovement ( map, 411, asc.MapCoordinate( 5, 15), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 23') 
  return
end 

--Move unit SHOCK TROOPER to #coord(5/10)#
r = asc.unitMovement ( map, 414, asc.MapCoordinate( 5, 10), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 24') 
  return
end 

--Attacking  #coord(5/11)# with SHOCK TROOPER
r = asc.unitAttack ( map, 414, asc.MapCoordinate(5, 11), 0 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 25') 
  return
end 
-- Ending turn 4
asc.endTurn()

--Move unit SHOCK TROOPER to #coord(5/13)#
r = asc.unitMovement ( map, 415, asc.MapCoordinate( 5, 13), -1 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 26') 
  return
end 

--Move unit SHOCK TROOPER to #coord(6/17)#
r = asc.unitMovement ( map, 411, asc.MapCoordinate( 6, 17), -1 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 27') 
  return
end 

--Move unit SHOCK TROOPER to #coord(6/12)#
r = asc.unitMovement ( map, 414, asc.MapCoordinate( 6, 12), 3 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 28') 
  return
end 

--Move unit SHOCK TROOPER to #coord(7/12)#
r = asc.unitMovement ( map, 374, asc.MapCoordinate( 7, 12), -1 )
if r:successful()==false then asc.displayActionError(r) 
  asc.errorMessage('Failure at command 29') 
  return
end 
