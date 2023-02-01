## Introduction and Background

A mass storage device such as a disk drive, solid state drive, or USB "stick" can be considered a large contiguous block of storage like memory.  Like memory, mass storage devices are accessed via an address, however mass storage devices address are for 'blocks' not bytes.  A read from a block device reads the entire block and a write, writes an entire block.  For mass storage devices, a block is typically 512 bytes.  The OS creates an abstraction of 'files' and 'directories' (folder) over the top of mass storage devices to give the user (and applications) the appearance of a file system.  A contemporary operating system such as Linux contains drivers for many file systems, with many being quite complex.

In this lab you will experiment with creating different file systems in Linux as well as interpret a file system architecture for a brand-new file system, teeny tiny file system (TTFS).

In Linux the file system is organized as a tree of files and directories starting at the root directory (/).  The "mount" command allows a system administrator to hang (mount) file systems from a branch of the tree.  Kind of like how you might hang an ornament on a tree during the holidays.  To mount a file system, you first need to create what is known as a "mount point" i.e., an empty directory that will serve as the entry point for the file system.

## Create and Mount a RAM Disk

A RAM disk creates a functional file system directly in random access memory (RAM).  Reads and writes to this file system will be an order of magnitude faster than mass storage device operations.  This fast access speed is a huge advantage.

What are the drawbacks?  1) Size – all file systems are limited based on the size of their backing media.  For a RAM disk that backing media is RAM which is on the order of a few gigabytes.  2) Persistence – When the system is shutdown or rebooted, the data stored in RAM is lost.  For a RAM disk this means that any files stored the file system will also be lost.

Why use a RAM disk then? Temporary files – many applications use short-lived files, either for inter process communication, storing intermediate results, or logging status.  For example, gcc runs through several phases to build an executable program: 1) running the processor, 2) generating assembly, 3) building object code 4) linking the final executable.  The output from each of these phases is one or more temporary files that are ultimately thrown away when the final executable is built.  Storing these intermediate files in a RAM disk may be able to increase compile speeds.  RAM disks are also used during boot to place a minimal system in memory which can then start loading the actual system.

To start, see how much free memory you have.

```text
user@pc:~$ free -h
              total        used        free      shared  buff/cache   available
Mem:           3.9G        263M        3.1G        7.8M        469M        3.4G
Swap:          4.3G          0B        4.3G
```

You should see two rows in the output.  One for physical memory and one for swap (the disk space reserved for the kernel to use for 'kicking out' pages from memory).

For a description of what each field in the table means you can run:  man free

There are two file system drivers that can be used for RAM disks, ramfs and tmpfs.  Large tmpfs disks can actually be swapped out to physical disk, which sort of defeats the purpose, but it is a little more full-featured than ramfs.

Now, make a mount point for the RAM disk.  /media is a common location for mount points.  To do this you will need administrator (root) authority.  The 'sudo' command allows you to temporarily escalate your system privileges to admin.  You'll need to type your password to escalate your privileges.

```text
user@pc:/$ sudo mkdir /media/ramdisk
```

Now we can mount the drive with the mount command and proper options.  The following will create a 512 MB RAM disk with file system type tmpfs in /media/ramdisk.  See man mount for full details.

```text
user@pc:/$ sudo mount -t tmpfs -o size=512m tmpfs /media/ramdisk
```

The disk is now usable.

Because the disk was made transiently, it will not be recreated on a reboot.  This is because Linux does not keep a record of file systems that were mounted when it shuts down.  There is, however, a file that Linux uses determine which file systems to mount at boot up.  This file is the file system table (fstab) and is locate in the /etc directory.  You can have a look opening /etc/fstab or just use the "cat" command to print it out.  You can have a RAM disk be created on every boot by adding an entry to /etc/fstab.  If you do this, the ram disk will be there on every boot, but any files present will be gone.

To complete this activity, perform the following and record your answers to the questions:

1. View the man page for free with "man free".  What does each column mean?  In your own words, what is the difference between free memory and available memory?
2. View the man page for mount with "man mount".  Use the information in the man page to record which file system types your Linux installation supports.
3. Pick one of the file systems supported by our Linux installation and learn more about how it works (use Google to help).  Give a summary of the file system.  Document your resources.
4. Create the RAM disk as outlined above.  What do each of the parameters to "mount" mean?
5. Record the output of free -h after creation of the RAM disk.  What is different?  Why?
6. Based on the observation of the free command before adding the ram disk, and after adding it and adding files to it, in which category of memory does the ram drive appear to reside?
7. Record the status of the RAM disk with the command df -h.  What can you learn about the file system by running df?
8. Copy or download a large file into the RAM disk.  I suggest a large pdf file (maybe around 25M).  Interact with this file (such as opening a pdf with a viewer) in the RAM disk as well as a copy on the normal file system.  What differences do you notice (e.g. performance, etc.)?  Why do you think these difference happen or not?
9. Record the output of free -h and df -h now that the RAM disk has a file in it.  How does the output compare to what you recorded before?  Does the output make sense?  Why?
10. Record the contents /etc/fstab.  What does each entry mean?  Consult "man fstab" for help.
11. What would an entry in /etc/fstab need to look like to create the 512MB tmpfs RAM disk when the system boots?

When done, the drive should be unmounted to tell Linux that it's no longer needed:

```text
user@pc:/media$ sudo umount /media/ramdisk 
```

Remember, that since this is a RAM disk, unmounting the file system will destroy the contents of the file system.

## Mount a File as a File System

Linux represents all block devices as a file in a special file system directory '/dev'.  This directory contains a representation of devices on the system.

List the contents of '/dev' on your Linux VM, you should see several 'files' sdaX (X will be a number).  These represent the disk partitions of the disk on your system.  Partitioning a disk is a way to create logical sub-disks that can each contain a file system that Linux can mount into the directory tree.  The 'file' in '/dev' will contain the binary representation of the file system.

The entire disk (all partitions) is represented by the file '/dev/sda'.  View the partitions created on the disk by running the following:

```text
user@pc:/$ sudo fdisk -l /dev/sda
```

Run the following to view the binary data by reading from the file:

```text
user@pc:/$ sudo hexdump -C /dev/sda1
```

This will print out the contents of the device '/dev/sda1' in hexadecimal.

Run 'mount' (with no arguments) to see all the file systems currently mounted on your Linux VM.  Several of them are RAM disks used for temporary file systems.  You should be able to see where your sdaX partitions are mounted.

While a file system will typically be backed by a physical device represented in '/dev', it doesn't have to be.  A file system can also be created using an ordinary file.  This could be used to create an image to be written to an optical disk or a removable flash drive, or to simply explore file systems without having to install and partition a physical hard drive.

Run the following to create a FAT16 image file:

```text
user@pc:~$ sudo mkdir /media/myimage
user@pc:~$ sudo mkdosfs -C -s 1 -S 512 -F 16 flash.img 5000
mkfs.fat 4.1 (2017-01-24)
```

This will create a file called flash.img which contains the raw bytes for a FAT16 file system with a sector size of 512 bytes and 5000 data blocks (a data block in FAT16 is 1024 bytes).  A disk drive is organized as an array of logical blocks indexed by logical block address (LBA), however this image file is a contiguous array of the bytes that would be placed in the logical blocks if it were to be stored on a disk drive.  On Linux, we can view the file bytes by using hexdump:

```text
hexdump –C flash.img
```

Now mount the image into the Linux directory tree and list its contents by running the following:

```text
user@pc:~$ sudo mount flash.img /media/myimage -o uid=YOURUSERNAME
user@pc:~$ cd /media/myimage/
user@pc:/media/myimage$ ls -al
total 20
drwxr-xr-x 2 root 0 16384 Dec 31  1969 .
drwxr-xr-x 8 root 0  4096 Nov  4 19:21 ..
```

NOTE: replace YOURUSERNAME with the name of your logged-in user.  

Now add some files to the file system image that you mounted.  You can use touch to create an empty file or cp to copy a file into the /media/myimage directory.  While this appears to be just creating or copying files into the directory, since the flash.img is mounted at /media/myimage, the files will be written to the file system image (flash.img) using the FAT16 file system driver.

**Specifically, add four text files, and delete two of them.  Also create at least one file with a long filename (longer than 8 characters).**

Unmount the file system with umount when done.  When you have completed the steps above answer the following questions:

1. Where are the mounted partitions of '/dev/sda' mounted?  Why do suspect that two partitions are used?  What is an advantage of partitioning the disk in this way?
2. Look at the image file with hexdump (hexdump –C flash.img | more).  NOTE: make sure the image is **unmounted** before you do this.  Can you find your files?  How about the ones you deleted?  How did you find them or why couldn't you?
3. Show a capture of the hexdump (of the flash.img) containing the directory entries for the files you added to the image as well as the ones you added and then deleted.  What is different?
4. Remount the image, change directory to the directory where the file system is mounted.  Now try to unmount the file system with "umount /media/myimage".  Did it work successfully?  Why or why not?
