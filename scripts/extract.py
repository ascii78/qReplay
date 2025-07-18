#!/usr/bin/python2
import xml.etree.ElementTree as ET
import json
import io

import sys

data="D:/Steam/steamapps/common/Wargame Red Dragon/Data/WARGAME/PC/510061340/"

# WGTableExporter.exe "D:\Steam\steamapps\common\Wargame Red Dragon\Data\WARGAME\PC\510060540\510061340\ZZ_Win.dat"

locale = {}

locales = io.open(data+"/ZZ_Win/pc/localisation/us/localisation/unites.csv", encoding="utf8")
for line in locales.readlines():
    chunk = line.split(",")
    locale[chunk[0]] = chunk[1].strip()

locale['null'] = "No translation"

tree = ET.parse(data+"/NDF_Win/pc/ndf/patchable/gfx/everything/TShowRoomDeckSerializer.xml")
root = tree.getroot()

trans_nato = {}
trans_pact = {}

for i in root[0].findall("OtanUnitIds"):
    for j in i:
        chunk = j.text.split()
        trans_nato[int(chunk[5])] = chunk[1]
        # print "0:%s:%s" % (chunk[1], chunk[5])

for i in root[0].findall("PactUnitIds"):
    for j in i:
        chunk = j.text.split()
        trans_pact[int(chunk[5])] = chunk[1]
        # print "1:%s:%s" % (chunk[1], chunk[5])

nato = {}
pact = {}

tree = ET.parse(data+"/NDF_Win/pc/ndf/patchable/gfx/everything/TUniteAuSolDescriptor.xml")
root = tree.getroot()

for i in root:
    uid = int(i.attrib["id"])
    name = locale[i.find("NameInMenuToken").text]
    cat = i.find("Factory").text
    price = []
    for j in i.find("ProductionPrice"):
        price.append(int(j.text))
    units = []
    for j in i.find("MaxDeployableAmount"):
        units.append(int(j.text))
    if i.find("Nationalite").text == "1":
        try:
            pact[trans_pact[uid]] = {"Name": name,
                                     "Category": cat,
                                     "Price": price,
                                     "Units": units}
        except KeyError:
            print(name, "not found for pact")
    else:
        try:
            nato[trans_nato[uid]] = {"Name": name,
                                     "Category": cat,
                                     "Price": price,
                                     "Units": units}
            if units[3] == 0 and units[4] == 1:
                print(name, units)
        except KeyError:
            print(name, "not found for nato")

tree = ET.parse(data+"/NDF_Win/pc/ndf/patchable/gfx/everything/TUniteDescriptor.xml")
root = tree.getroot()

for i in root:
    uid = int(i.attrib["id"])
    name = locale[i.find("NameInMenuToken").text]
    cat = i.find("Factory").text
    price = []
    for j in i.find("ProductionPrice"):
        price.append(int(j.text))
    units = []
    for j in i.find("MaxDeployableAmount"):
        units.append(int(j.text))
    if i.find("Nationalite").text == "1":
        try:
            pact[trans_pact[uid]] = {"Name": name,
                                     "Category": cat,
                                     "Price": price,
                                     "Units": units}
        except KeyError:
            pass
    else:
        try:
            nato[trans_nato[uid]] = {"Name": name,
                                     "Category": cat,
                                     "Price": price,
                                     "Units": units}
        except KeyError:
            pass


a = open("units.json", "w")
units = {0: nato, 1: pact}
json.dump(units, a, sort_keys=True, indent=4, separators=(',', ': '))
a.close()
