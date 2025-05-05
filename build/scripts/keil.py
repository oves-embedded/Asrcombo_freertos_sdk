# Copyright (c) 2022 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import sys
import string
import re, shutil
import importlib
import xml.etree.ElementTree as etree
from xml.etree.ElementTree import SubElement
from xml_format import gen_indent

# Global  Definitions
TEMPLATE_PROJX = "../build/scripts/asr_template.uvprojx"
TEMPLATE_OPTX = "../build/scripts/asr_template.uvoptx"

config = "asr_config"
outputname_string = ""
# Elements need to be changed:
element_dict = {
    "TargetName": { "xpath": "Targets/Target/TargetName" },
    "Device": { "xpath": "Targets/Target/TargetOption/TargetCommonOption/Device" },
    "Vendor": { "xpath": "Targets/Target/TargetOption/TargetCommonOption/Vendor" },
    "OutputName": { "xpath": "Targets/Target/TargetOption/TargetCommonOption/OutputName" },
    "CreateExecutable": { "xpath": "Targets/Target/TargetOption/TargetCommonOption/CreateExecutable" },
    "CreateLib": { "xpath": "Targets/Target/TargetOption/TargetCommonOption/CreateLib" },
    "RunUserProg1": { "xpath": "Targets/Target/TargetOption/TargetCommonOption/AfterMake/RunUserProg1" },
    "RunUserProg2": { "xpath": "Targets/Target/TargetOption/TargetCommonOption/AfterMake/RunUserProg2" },
    "UserProg1Name": { "xpath": "Targets/Target/TargetOption/TargetCommonOption/AfterMake/UserProg1Name" },
    "UserProg2Name": { "xpath": "Targets/Target/TargetOption/TargetCommonOption/AfterMake/UserProg2Name" },
    "ScatterFile": { "xpath": "Targets/Target/TargetOption/TargetArm/LDarm/ScatterFile" },
    "Misc": { "xpath": "Targets/Target/TargetOption/TargetArm/LDarm/Misc" },
    "GCPUTYP": { "xpath": "Targets/Target/TargetOption/TargetArm/ArmMisc/GCPUTYP" },
    "ADefine": { "xpath": "Targets/Target/TargetOption/TargetArm/Aarm/VariousControls/Define" },
    "Define": { "xpath": "Targets/Target/TargetOption/TargetArm/Carm/VariousControls/Define" },
    "IncludePath": { "xpath": "Targets/Target/TargetOption/TargetArm/Carm/VariousControls/IncludePath" },
    "MiscControls_cflags": { "xpath": "Targets/Target/TargetOption/TargetArm/Carm/VariousControls/MiscControls" },
    "MiscControls_asmflags": { "xpath": "Targets/Target/TargetOption/TargetArm/Aarm/VariousControls/MiscControls" },
}

def create_file(data, filename):
    """ Create *_opts files """
    with open(filename, "w") as f:
        f.write(data)

def get_element_value(element_dict, target):
    global outputname_string
    """Get elements value """
    # TargetName, OutputName = app@board
    element_dict["TargetName"]["value"] = target
    element_dict["OutputName"]["value"] = outputname_string

    element_dict["CreateExecutable"]["value"] = config.createexe
    element_dict["CreateLib"]["value"] = config.createLib

    element_dict["RunUserProg1"]["value"] = config.runuser1
    element_dict["RunUserProg2"]["value"] = config.runuser2

    element_dict["UserProg1Name"]["value"] = config.runuser1_pro.replace("/","\\")
    element_dict["UserProg2Name"]["value"] = config.runuser2_pro.replace("/","\\")

    # Device = $(NAME)_KEIL_DEVICE that defined in board makefile
    element_dict["Device"]["value"] = 'ARMCM4_FP'
    # Vendor = $(NAME)_KEIL_VENDOR that defined in board makefile
    element_dict["Vendor"]["value"] = 'ARM'

    # ScatterFile = the matched part in LDFLAGS "--scatter=(*.sct)"
    # Misc = global ldflags

    element_dict["ScatterFile"]["value"] = config.ld_files.replace("/","\\")

    element_dict["Misc"]["value"] = config.ld_misc

    # GCPUTYP = HOST_ARCH that defined in board makefile
    element_dict["GCPUTYP"]["value"] = config.host_arch
    # include path
    element_dict["IncludePath"]["value"] = config.include_path.replace("/","\\")

    # Define = global defines splitted by ","
    element_dict["Define"]["value"] = config.defines.replace("-D","")

    element_dict["ADefine"]["value"] = config.adefines.replace("-D","")
    # MiscControls_cflags = global cflags
    element_dict["MiscControls_cflags"]["value"] = config.cMisc

    # MiscControls_asmflags = global asmflags
    element_dict["MiscControls_asmflags"]["value"] = config.aMisc

def file_type_value(fn):
    """ Mapping Number and Filetype """
    if fn.endswith('.h'):
        return 5
    if fn.endswith('.s') or fn.endswith('.S'):
        return 2
    if fn.endswith('.lib') or fn.endswith('.a'):
        return 4
    if fn.endswith('.cpp') or fn.endswith('.cxx'):
        return 8
    if fn.endswith('.c') or fn.endswith('.C'):
        return 1
    return 5

def add_group(parent, name, files, project_path):
    """Create Group SubElenent"""
    cur_encoding = sys.getfilesystemencoding()
    group = SubElement(parent, 'Group')
    group_name = SubElement(group, 'GroupName')
    group_name.text = name
    files_label = SubElement(group, 'Files')
    for f in files:
        file = SubElement(files_label, 'File')
        file_name = SubElement(file, 'FileName')
        name = os.path.basename(f)
        file_name.text = name.encode('utf-8').decode(cur_encoding)
        file_type = SubElement(file, 'FileType')
        file_type.text = '%d' % file_type_value(name)
        file_path = SubElement(file, 'FilePath')
        f = f.replace('/','\\')
        file_path.text = f.encode('utf-8').decode(cur_encoding)
    return group

def changeItemForMcu(tree, xpath, value):
    """Set Element's value"""
    element = tree.find(xpath)
    element.text = value

def gen_file_group():
    """Generate group"""
    group_pack = []
    no_group_counter = 0
    group_ = config.group
    src_files = config.src
    for gr_name in group_:
        group_dict={}
        group_dict['src'] = []
        group_dict['name'] = gr_name
        for src in src_files:
            src_tmp = src[0:src.rfind('/')+1]
            if gr_name in src_tmp:
                group_dict['src'].append(src)
        group_pack.append(group_dict)

    group_dict={}
    group_dict['name'] = 'pro'
    group_dict['src'] = []
    for root_src in config.src:
        for group_entry in group_pack:
            if root_src not in group_entry['src']:
                no_group_counter = no_group_counter +1
                continue
            else:
                no_group_counter=0
                break;
        if no_group_counter != 0 and no_group_counter == len(group_pack):
            group_dict['src'].append(root_src)
        no_group_counter=0
    group_pack.append(group_dict)
    return group_pack

def gen_projxfile(tree, target, buildstring, project_group):
    project_path = os.path.dirname(os.path.abspath(target))

    root = tree.getroot()
    get_element_value(element_dict, buildstring)

    for key in element_dict:
        xpath = element_dict[key]["xpath"]
        value = element_dict[key]["value"]
        changeItemForMcu(tree, xpath, value)
    # add group
    groups = tree.find('Targets/Target/Groups')
    if groups is None:
        groups = SubElement(tree.find('Targets/Target'), 'Groups')
    groups.clear() # clean old groups

    lib_group=[]
    src_group=[]
    for group_lite in project_group:
        if 'lib' in group_lite['name']:
            lib_group.append(group_lite)
        else:
            src_group.append(group_lite)

    for single_group in src_group:
        # don't add an empty group
        if len(single_group['src'])!= 0:
            group_tree = add_group(groups, single_group['name'],single_group['src'] , project_path)
    for single_group in lib_group:
        # don't add an empty group
        if len(single_group['src'])!= 0:
            group_tree = add_group(groups, single_group['name'],single_group['src'] , project_path)

    gen_indent(root)

    with open(target, 'wb') as f:
        f.write('<?xml version="1.0" encoding="UTF-8" standalone="no" ?>\n'.encode())
        f.write(etree.tostring(root, encoding='utf-8'))

def gen_optxfile(optx_tree, optx_file, buildstring):
    TargetName = optx_tree.find('Target/TargetName')
    TargetName.text = buildstring
    if config.ini_files:
        TargetName = optx_tree.find('Target/TargetOption/DebugOpt/tIfile')
        TargetName.text = config.ini_files.replace("/","\\")
    with open(optx_file, "wb") as f:
        f.write(etree.tostring(optx_tree.getroot(), encoding='utf-8'))

def main():
    global config,outputname_string
    # buildstring, eg: ate_lib
    buildstring = sys.argv[1]
    outputname_string = sys.argv[2]

    target_path = './ide/keil/'+buildstring
    config_module = buildstring + '_config'

    config = importlib.import_module(config_module)

    if os.path.exists(target_path):
        shutil.rmtree(target_path)

    os.makedirs(target_path)

    projx_file = "./ide/keil/%s/%s.uvprojx" % (buildstring, buildstring)

    optx_file = projx_file.replace('.uvprojx', '.uvoptx')

    print ("Creating keil project %s" % (buildstring))

    projx_tree = etree.parse(TEMPLATE_PROJX)
    optx_tree = etree.parse(TEMPLATE_OPTX)

    pro_group = gen_file_group()

    # create uvprojx file
    gen_projxfile(projx_tree, projx_file, buildstring, pro_group)

    # create uvoptx file
    gen_optxfile(optx_tree, optx_file, buildstring)

    print ("Keil project created at %s" % (projx_file))

if __name__ == "__main__":
    main()
