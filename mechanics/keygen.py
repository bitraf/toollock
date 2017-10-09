def bitpattern(number):
    return map(int, bin(number)[2:])

def keypattern(number, length=5):
    pattern = bitpattern(number)
    pattern = [0]*(length-len(pattern)) + pattern
    return pattern

def crossection(obj, height, plane=None, doc=None):
    if doc is None:
        doc = App.activeDocument()
    if plane is None:
        plane = FreeCAD.Vector(0,0,1)

    wires = obj.Shape.slice(plane, height)
    slice = doc.addObject("Part::Feature","CrossSection")
    slice.Shape = Part.Compound(wires)
    return slice

#crossection(key)

def make_keypair(number, lock, key, notch, doc=None):
    if doc is None:
        doc = App.activeDocument()

    keybumps = []
    lockbumps = []
    notchwidth = 4
    notchheight = 2
    for i, bit in enumerate(keypattern(number, 5)):
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

    key.ViewObject.Visibility = True
    lock.ViewObject.Visibility = True

    # make cross-sections
    keyslice = crossection(keyb, 7)
    lockslice = crossection(lockb, 7)

    for obj in temp:
        doc.removeObject(obj.Name)

    return keyslice, lockslice

def make_keys(locktemplate, keytemplate, notch, first=0, last=31, doc=None):
    if doc is None:
        doc = App.activeDocument()

    font = "/usr/share/fonts/noto/NotoSansMono-Regular.ttf"
    fontheight = 4.0 # in mm

    top = doc.addObject("App::DocumentObjectGroup", "Keyset")
    texts = doc.addObject("App::DocumentObjectGroup", "Texts")
    keys = doc.addObject("App::DocumentObjectGroup", "Keys")
    locks = doc.addObject("App::DocumentObjectGroup", "Locks")
    top.addObject(texts)
    top.addObject(keys)
    top.addObject(locks)

    for i, num in enumerate(range(first, last+1)):
        key, lock = make_keypair(num, locktemplate, keytemplate, notch)
        lockP = lock.Placement.Base + FreeCAD.Vector(100 +  i*70, 0, 0)
        keyP = key.Placement.Base + FreeCAD.Vector(100 + i*70, -50, 0)

        lock.Placement.Base = lockP
        key.Placement.Base = keyP 
        keys.addObject(key)
        locks.addObject(lock)

        keyname = 'A{:02d}'.format(num)

        keytext = Draft.makeShapeString(keyname,FontFile=font,Size=fontheight,Tracking=0)
        keytext.Placement.Base = keyP + FreeCAD.Vector(-7, 2, 0)
        texts.addObject(keytext)

        locktext = Draft.makeShapeString(keyname,FontFile=font,Size=fontheight,Tracking=0)
        locktext.Placement.Base = lockP + FreeCAD.Vector(-7, 48, 0)
        texts.addObject(locktext)

make_keys(lock, key, notch, first=29, last=31)
