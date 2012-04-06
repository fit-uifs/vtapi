#!/usr/bin/env python

# Dataset loading script using VTApi.
# Script recursively searches down a specified directory for image/video files.
# In next step, it invokes commands for adding sequences (videos/imagefolders)
# and intervals (images). Script uses command-line interface of VTApi (vtcli)

# Usage: vtapi_load.py ds_name ds_folder [vtcli_bin]
#       ex.: ./vtapi_load.py sunar /mnt/data/sunar ../vtcli

import os
import sys
import mimetypes

VTCLI_BIN = "../dist/vtapi"
DEBUG = 1   # 1=print, 0=run

########## MAIN
def main():
    global VTCLI_BIN
    if len(sys.argv) < 3:
        print "ERROR: 2 argument needed"
        print_help()
        exit(2)
    else:
        ds_name = sys.argv[1]
        ds_folder = sys.argv[2]
        if ds_folder.endswith(os.sep) == False:
            ds_folder += os.sep

    if len(sys.argv) > 3:
        VTCLI_BIN = sys.argv[3]

    # remember folders with images in it, don't add twice
    image_folders = {}

    # browse files
    for top,dirs,files in os.walk(ds_folder):
        for file in files:

            # get MIME type
            full_path = os.path.join(top,file)
            mime_type, enc = mimetypes.guess_type(full_path)
            if mime_type:
                mime_base = mime_type.split('/')[0]
            else:
                continue

            # add video with the name of its full filepath
            if mime_base == "video":
                location = os.path.join(top.replace(ds_folder,"",1),file)
                add_video(ds_name, location, location)

            # add image folder as sequence (only once) and image as interval
            elif mime_base == "image":
                seq_name = top.replace(ds_folder,"",1)
                seq_location = seq_name + os.sep
                # add new image folder
                if not (seq_name in image_folders.keys()):
                    add_imagefolder(ds_name, seq_name, seq_location)
                    image_folders[seq_name] = 0
                # increment image counter in folder and add image
                image_folders[seq_name] += 1
                add_image(ds_name, seq_name, image_folders[seq_name], file)
        #end for
    #end for
    return

# add video command (insert sequence type=video ...)
def add_video(ds_name, name, location):
    cmd = VTCLI_BIN + " -d" + ds_name +\
    " insert sequence name=" + name + " location=" + location + " type=video" 
    if DEBUG:
        print cmd
    else:
        os.system(cmd)
    return

# add image folder command (insert sequence type=images ...)
def add_imagefolder(ds_name, name, location):
    cmd = VTCLI_BIN + " -d" + ds_name +\
    " insert sequence name=" + name + " location=" + location + " type=images"
    if DEBUG:
        print cmd
    else:
        os.system(cmd)
    return

# add image command (insert interval ...)
def add_image(ds_name, seqname, t, location):
    cmd = VTCLI_BIN + " -d" + ds_name +\
    " insert interval seqname=" + seqname + " t1=" + str(t) + " t2=" + str(t) +\
    " location=" + location
    if DEBUG:
        print cmd
    else:
        os.system(cmd)
    return

# help
def print_help():
    print "Usage: vtapi_load.py ds_name ds_folder [vtcli_bin]\n"
    print "     ds_name...   name of the dataset"
    print "   ds_folder...   folder with data"
    print "   vtcli_bin...   VTCli executable (def. \"../dist/vtapi\")"
    return

# main
if __name__ == "__main__":
    main()