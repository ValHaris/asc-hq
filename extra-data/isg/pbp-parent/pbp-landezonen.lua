-- -----------------------------------------------
-- Erstellen der in PBP verwendeten Landezonen
--
-- lauffähig im Mapeditor
-- -----------------------------------------------


-- benötigte Funktionen definieren
-- -----------------------------------------------

-- setzt ein Objekt an die angegebene Position
function setObject ( position, objektid )

	local objekt	= asc.getObjectType ( objektid )
	asc.placeObject( map, position, objekt, true )	

end

-- setzt eine Anzahl von Objekten an eine angegebene Position
function setObjectArray ( position, objektarray )

	i	= 1
	while objektarray[i] ~= nil do
		setObject ( position, objektarray[i] )
		i	= i + 1
	end
	
end

-- setzt in einem Ring mit einem Radius X um eine definierte Position eine Anzahl von Objekten
function setRingObjectArray ( position, objektarray, range )

	fields	= asc.getFieldsInDistance( map, position, range )
	for j = 1, fields:size()  do
		pos = fields:getItem(j)
		setObjectArray ( pos, objektarray )
	end
	
end

-- setzt eine Einheit an die angegebene Position mit dem angegebenem Eigentümer
function setUnit ( position, unitid, owner )

	local unit	= asc.getUnitType ( unitid )
	asc.placeUnit( map, position, unit, owner )	

end

-- Funktion um ein Gebäude (ID) auf ein Feld zu setzen
function setBuilding ( position, buildingid, owner )

	local building	= asc.getBuildingType ( buildingid )
	asc.placeBuilding( map, position, building, owner )

end

-- Funktion um Bauoptionen in ein Gebäude einzufügen
function setOptions ( position, options )

	building	= map:getField( position ):getBuildingEntrance()
	i	= 1
	while options[i] ~= nil do
		building:addProductionLine( asc.getUnitType ( options[i] ) )
		i	= i + 1
	end

end


-- benötigte Variablen definieren
-- -----------------------------------------------

objekts		= {}		-- Landezonen Untergrund
options		= {}		-- Bauoptionen

table.insert( objekts, 12012 )

lzdepot		= -1		-- vordefiniert

unit_cu		= 12006		-- Command Unit
unit_jam	= 12010		-- ISG Jammer
unit_radar	= 12011		-- ISG Invasor Radar
unit_sv         = 12001         -- ILZ Wächter

objekt_sock	= 12013		-- ISG Sockel
objekt_lz	= 12015		-- ISG Landezone

bld_depot	= 12001		-- ISG Depot
bld_hbhq	= 12025		-- HB HQ
bld_kolrad	= 12026		-- Kolonie Radar
bld_kolrz	= 12027		-- Raumfahrtzentrum

inactiv = { 12502 ,
            12509 ,
            12511 ,
            12518 ,
            12519 ,
            12522 ,
            12523 ,
            12529 ,
            12533 ,
            12534 ,
            12535 ,
            12539 ,
            12543 ,
            12546 ,
            12547 ,
            12554 }

-- das eigentliche Skript
-- -----------------------------------------------

-- ein Objekt der aktuellen Karte laden
map 		= asc.getActiveMap()

-- aktuelle Cursorposition bestimmen
centerpos 	= asc.getCursorPosition( map )

-- Dialoge vorbereiten
-- -----------------------------------------------

dlglztype	= asc.StringArray()
dlglztype:add('Heimatbasis Komplex')
dlglztype:add('Kolonie Komplex')
dlglztype:add('Invasionslandezone (ILZ)')
-- dlglztype:add('temporäre Invasionslandezone')

dlgplayer	= asc.StringArray()
dlgplayer:add('Spieler 0 (SV)')
dlgplayer:add('Spieler 1')
dlgplayer:add('Spieler 2')
dlgplayer:add('Spieler 3')
dlgplayer:add('Spieler 4')
dlgplayer:add('Spieler 5')
dlgplayer:add('Spieler 6')
dlgplayer:add('Spieler 7')

dlglzbeton	= asc.PropertyDialog('Welche Objekte sollen mit platziert werden?')
dlglzbeton:addBool('Pipelines', true )
dlglzbeton:addBool('Straße', true )
dlglzbeton:addBool('Schiene', true )
dlglzbeton:addBool('Runway', false )
dlglzbeton:addBool('ISG Depot', true )

--Dialog Auswahl Koloniestandort
dlgkolodir	= asc.StringArray()
dlgkolodir:add('Kolonie (oberer Rand)')
dlgkolodir:add('Kolonie (unterer Rand)')

--Dialog Auswahl Invasionslandezone Standort
dlglzdir	= asc.StringArray()
dlglzdir:add('ILZ (unterer Rand) ')
dlglzdir:add('ILZ (oberer Rand) ')
dlglzdir:add('ILZ (linker Rand) ')
dlglzdir:add('ILZ (rechter Rand) ')

dlgdepot	= asc.StringArray()
dlgdepot:add('ISG Depot im Nordosten')
dlgdepot:add('ISG Depot im Südosten')
dlgdepot:add('ISG Depot im Südwesten')
dlgdepot:add('ISG Depot im Nordwesten')

dlgoptions	= asc.PropertyDialog('Welche Bauoptionen sollen mit eingefügt werden?')
dlgoptions:addBool('ISG Builder', true )
dlgoptions:addBool('ISG Carrier', true )
dlgoptions:addBool('Standard Aircraft Carrier', true )
dlgoptions:addBool('Standard Heavy Fighter', true )
dlgoptions:addBool('Standard Satellite', true )
--dlgoptions:addBool('personal unit', false )

-- Dialoge abarbeiten
-- -----------------------------------------------

lztype	= asc.selectString('Art der Landezone auswählen!', dlglztype )

if lztype < 0 then
	asc.warningMessage("Skript Abruch -> keine Art der Landezone ausgewählt!")
	return
end

lzowner	= asc.selectString('Besitzer der Landezone festlegen!', dlgplayer )
if lzowner == -1 then
	asc.warningMessage("Skript Abruch -> keinen Besitzer der Landezone ausgewählt!")
	return
end

-- Art des Untergrundes festlegen
if lztype == 0 or lztype == 1 or lztype == 3 then
	result	= dlglzbeton:run()
	
	if result == false then
		asc.warningMessage("Skript Abruch -> Art des Untergrundes konnte nicht bestimmt werden!!")
		return
	end
	
	if dlglzbeton:getBool('Pipelines') == true then
		table.insert( objekts, 30 )
	end
	
	if dlglzbeton:getBool('Straße') == true then
		table.insert( objekts, 1 )
	end
	
	if dlglzbeton:getBool('Schiene') == true then
		table.insert( objekts, 2 )
	end
	
	if dlglzbeton:getBool('Runway') == true then
		table.insert( objekts, 45 )
	end
end

-- Ausrichtung des Depots festlegen
if lztype == 0 or lztype == 1 then
	if dlglzbeton:getBool('ISG Depot') == true then
		lzdepot	= asc.selectString('Position des ISG Depot festlegen!', dlgdepot )

		if lzdepot < 0 then
			asc.warningMessage("Skript Abruch -> Die Position des ISG Depot konnte nicht bestimmt werden!!")
			return	
		end
	end
end

-- Ausrichtung des Kolonie RZ festlegen
if lztype == 1 then
	kolodir	= asc.selectString('Position des Raumfahrzentrum (RZ) festlegen', dlgkolodir )

	if kolodir < 0 then
		asc.warningMessage("Skript Abruch -> Die Position des RZ konnte nicht bestimmt werden!!")
		return	
	end
end

if lztype == 2 then
	lzdir	= asc.selectString('Position der CommandUnit (CU) festlegen', dlglzdir )

	if lzdir < 0 then
		asc.warningMessage("Skript Abruch -> Die Position der CU konnte nicht bestimmt werden!!")
		return	
	end
end

-- Bauoptionen festlegen
if lztype == 0 or lztype == 1 then
	result	= dlgoptions:run()
	if result == false then
		asc.warningMessage("Skript Abruch -> Die Bauoptionen konnte nicht bestimmt werden!!")
		return
	end	
	
	if dlgoptions:getBool('ISG Builder') == true then
		table.insert( options, 12005 )
	end
	
	if dlgoptions:getBool('ISG Carrier') == true then
		table.insert( options, 12004 )
	end	
	
	if dlgoptions:getBool('Standard Aircraft Carrier') == true then
		table.insert( options, 12008 )
	end	
	
	if dlgoptions:getBool('Standard Heavy Fighter') == true then
		table.insert( options, 12009 )
	end

	if dlgoptions:getBool('Standard Satellite') == true then
		table.insert( options, 12002 )
	end	
--	if dlgoptions:getBool('personal unit' ) == true then
--
--          dlgPUs = asc.PropertyDialog('Welche personal units sollen mit eingefügt werden?')
--	   for PUID = 12500 , 12999 do
--              dlgPUs:addBool( PUID , true )
--           end
--           PUdir = asc.selectString('Position der CommandUnit (CU) festlegen', dlgPUs )
--	end	
end

-- HQ Komplex setzen
-- -----------------------------------------------
if lztype == 0 then
	setObjectArray ( centerpos, objekts )
	setRingObjectArray ( centerpos, objekts, 1 )
	setRingObjectArray ( centerpos, objekts, 2 )
	setRingObjectArray ( centerpos, objekts, 3 )

	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 3 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
	asc.clearField( map, pinselpos )

	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 0 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 0 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 0 )
	asc.clearField( map, pinselpos )
	
	setObject ( centerpos, objekt_lz )	
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 0 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 1 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 2 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 3 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 4 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 5 )
	setObject ( pinselpos, objekt_lz )
	
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 0 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 1 )
	setObject ( pinselpos, objekt_sock )
	setUnit ( pinselpos, unit_jam, lzowner )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )
	setObject ( pinselpos, objekt_sock )

	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 2 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )
	setObject ( pinselpos, objekt_sock )
	setUnit ( pinselpos, unit_jam, lzowner )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 4 )
	setObject ( pinselpos, objekt_sock )

	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 4 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 4 )
	setObject ( pinselpos, objekt_sock )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )
	setObject ( pinselpos, objekt_sock )
	setUnit ( pinselpos, unit_jam, lzowner )

	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 5 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 5 )
	setObject ( pinselpos, objekt_sock )
	setUnit ( pinselpos, unit_jam, lzowner )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 1 )
	setObject ( pinselpos, objekt_sock )

	setBuilding ( centerpos, bld_hbhq, lzowner )
	setOptions ( centerpos, options )

	if lzdepot == 0 then
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 1 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 1 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 1 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 0 )
	elseif lzdepot == 1 then
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 2 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
	elseif lzdepot == 2 then
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 4 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 4 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 4 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
	elseif lzdepot == 3 then
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 5 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 5 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 5 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 0 )
	end
	
	if lzdepot ~= -1 then
		setBuilding ( pinselpos, bld_depot, 0 )
	end
end

-- Kolonie Landezone setzen
-- -----------------------------------------------
if lztype == 1 then
	setObjectArray ( centerpos, objekts )
	setRingObjectArray ( centerpos, objekts, 1 )
	setRingObjectArray ( centerpos, objekts, 2 )

        -- Felder links nachsetzen
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 5 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 5 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 4 )
	setObjectArray ( pinselpos, objekts )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
	setObjectArray ( pinselpos, objekts )

	-- Felder rechts nachsetzen
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 1 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 1 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )
	setObjectArray ( pinselpos, objekts )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
	setObjectArray ( pinselpos, objekts )

        -- Landepads hinzufügen
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 1 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 2 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 4 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 5 )
	setObject ( pinselpos, objekt_lz )	

        -- 2 Sockel links und rechts hinzufügen
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 5 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 4 )	
	setObject ( pinselpos, objekt_sock )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 1 )
	pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )	
	setObject ( pinselpos, objekt_sock )

        -- Kolonie-Radar
	setBuilding ( centerpos, bld_kolrad, lzowner )

        -- RZ an die richtige Stelle setzen + Bauoptionen 
	if kolodir == 0 then
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 0 )
	else
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 3 )
	end
	setBuilding ( pinselpos, bld_kolrz, lzowner )
	setOptions ( pinselpos, options )
	
	-- ISG Depot setzen
	if lzdepot == 0 then
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 1 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 1 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 1 )
	elseif lzdepot == 1 then
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 2 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )
	elseif lzdepot == 2 then
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 4 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 4 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 4 )
	elseif lzdepot == 3 then
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 5 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 5 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 5 )
	end
	
	if lzdepot ~= -1 then
		setBuilding ( pinselpos, bld_depot, 0 )
	end
end


-- feste Invasorzone setzen
-- -----------------------------------------------
if lztype == 2 then
   if lzdir == 0 or lzdir == 1 then
	setObjectArray ( centerpos, objekts )
	setRingObjectArray ( centerpos, objekts, 1 )
	setRingObjectArray ( centerpos, objekts, 2 )

	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 1 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 2 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 4 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 5 )
	setObject ( pinselpos, objekt_lz )

	setUnit ( centerpos, unit_radar, lzowner )

	if lzdir == 0 then
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 3 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
		asc.clearField( map, pinselpos )
		
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 0 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 0 )
		pinselpos1	= asc.getNeighbouringFieldCoordinate( pinselpos, 5 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 1 )
		
		setObjectArray ( pinselpos1, objekts )
		setObjectArray ( pinselpos, objekts )
		
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 0 )
	else
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 0 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 0 )
		asc.clearField( map, pinselpos )
		
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 3 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
		pinselpos1	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 4 )
		
		setObjectArray ( pinselpos1, objekts )
		setObjectArray ( pinselpos, objekts )

		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 3 )
	end

	setUnit ( pinselpos, unit_cu, lzowner )
   end
end

if lztype == 2 then
   if lzdir == 2 or lzdir == 3 then
	setObjectArray ( centerpos, objekts )
	setRingObjectArray ( centerpos, objekts, 1 )
	setRingObjectArray ( centerpos, objekts, 2 )

	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 1 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 2 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 4 )
	setObject ( pinselpos, objekt_lz )
	pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 5 )
	setObject ( pinselpos, objekt_lz )

	setUnit ( centerpos, unit_radar, lzowner )

	if lzdir == 2 then
                -- Felder oben nachsetzen
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 5 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 0 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 0 )
		setObjectArray ( pinselpos, objekts )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 4 )
		setObjectArray ( pinselpos, objekts )

                -- Felder unten nachsetzen		
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 4 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
		setObjectArray ( pinselpos, objekts )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 5 )
		setObjectArray ( pinselpos, objekts )
		
		-- Felder rechts nachsetzen
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 1 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 1 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )
		setObjectArray ( pinselpos, objekts )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
		setObjectArray ( pinselpos, objekts )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 1 )
		setObjectArray ( pinselpos, objekts )
                
                -- CU und SV setzen setzen
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 1 )
	        setUnit ( pinselpos, unit_sv, lzowner )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )
                setUnit ( pinselpos, unit_cu, lzowner )
	else
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 1 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 0 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 0 )
		setObjectArray ( pinselpos, objekts )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 2 )
		setObjectArray ( pinselpos, objekts )

                -- Felder unten nachsetzen		
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 2 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
		setObjectArray ( pinselpos, objekts )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 1 )
		setObjectArray ( pinselpos, objekts )
		
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 5 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 5 )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 4 )
		setObjectArray ( pinselpos, objekts )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 3 )
		setObjectArray ( pinselpos, objekts )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 5 )
		setObjectArray ( pinselpos, objekts )

                -- CU und SV setzen setzen
		pinselpos	= asc.getNeighbouringFieldCoordinate( centerpos, 5 )
	        setUnit ( pinselpos, unit_sv, lzowner )
		pinselpos	= asc.getNeighbouringFieldCoordinate( pinselpos, 4 )
                setUnit ( pinselpos, unit_cu, lzowner )
	end

   end
end
