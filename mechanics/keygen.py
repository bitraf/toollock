def bitpattern(number):
    return map(int, bin(number)[2:])

def crossection(obj, height, plane=None, doc=None):
    if doc is None:
        doc = App.activeDocument()
    if plane is None:
        plane = FreeCAD.Vector(0,0,1)

    wires = obj.Shape.slice(plane, height)
    slice = doc.addObject("Part::Feature","CrossSection")
    slice.Shape = Part.Compound(wires)
    #slice.purgeTouched()
    return slice

#crossection(key)

def make_keypair(number, lock, key, notch, doc=None):
    if doc is None:
        doc = App.activeDocument()

    keybumps = []
    lockbumps = []
    notchwidth = 4
    notchheight = 2
    for i, bit in enumerate(bitpattern(number)):
        if bit:
            offset = notch.Placement.Base + FreeCAD.Vector(-i*notchwidth, 0, 0)
            keybumps.append(Draft.clone(notch, offset))
        else:
            offset = notch.Placement.Base + FreeCAD.Vector(-i*notchwidth, notchheight, 0)
            b = Draft.clone(notch, offset)
            b.Placement = App.Placement(offset, App.Rotation(FreeCAD.Vector(0,0,1),180))
            lockbumps.append(b)

    keyb = doc.addObject("Part::MultiFuse", "Fusion")
    keyb.Shapes = keybumps + [key]

    lockb = doc.addObject("Part::MultiFuse", "Fusion")
    lockb.Shapes = lockbumps + [lock]

    temp = [ keyb, lockb ] + keybumps + lockbumps

    doc.recompute() # otherwise fusions don't update??

    # make cross-sections
    keyslice = crossection(keyb, 7)
    lockslice = crossection(lockb, 7)

    for obj in temp:
        doc.removeObject(obj.Name)

    return keyslice, lockslice

def make_keys(locktemplate, keytemplate, notch, start=0, end=31, doc=None):
    for i, num in enumerate(range(start, end)):
        key, lock = make_keypair(num, locktemplate, keytemplate, notch)

        lock.Placement.Base = lock.Placement.Base + FreeCAD.Vector(100 +  i*70, 0, 0)
        key.Placement.Base = key.Placement.Base + FreeCAD.Vector(100 + i*70, -50, 0)

make_keys(lock, key, notch, start=29, end=31)
