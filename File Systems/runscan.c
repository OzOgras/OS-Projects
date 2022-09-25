#include <stdio.h>
#include "ext2_fs.h"
#include "read_ext2.h"
#include <math.h>
#include <sys/types.h>
#include <dirent.h>

#define MAX_STRING_SIZE 128
int main(int argc, char **argv) {
	
	if (argc != 3) {
            printf("expected usage: ./runscan inputfile outputfile\n");
            exit(0);
	}
        
	int fd;

	fd = open(argv[1], O_RDONLY);    /* open disk image */

	// Open directory
	// DIR *dp = 
	// opendir(argv[2]);
	// char pName[MAX_STRING_SIZE];
	// sprintf(pName, argv[2]);
	
	char *dir = argv[2];
	mkdir(dir, S_IRWXU);
	
	ext2_read_init(fd);

	struct ext2_super_block super;
	struct ext2_group_desc group;
	
	// example read first the super-block and group-descriptor
	read_super_block(fd, 0, &super);
	// read_group_desc(fd, 0, &group);
	
	printf("There are %u inodes in an inode table block and %u blocks in the idnode table\n", inodes_per_block, itable_blocks);

	// Calculate the number of group blocks there are
	unsigned int group_count = 1 + (super.s_blocks_count-1) / super.s_blocks_per_group;
	printf("The groupCount is %d\n", group_count);

	for (unsigned int groupID = 0; groupID < group_count; groupID++) {
	   
	   read_group_desc(fd, groupID, &group);
	   // Gets the offset of the start location of the inode table (Oz)
	   off_t start_inode_table = locate_inode_table(groupID, &group);

	   //off_t start_data_block = BLOCK_OFFSET(group->bg_inode_table + itable_blocks); 
           // unsigned int  start_data_block = group.bg_inode_table + itable_blocks;

	   // iterate the first inode block
	   for (unsigned int i = 0; i < itable_blocks; i++) {
		printf("inode %u: \n", i);
		struct ext2_inode *inode = malloc(sizeof(struct ext2_inode));
		// Reads the inode table(Oz)
		
		read_inode(fd, 0, start_inode_table,i, inode);

		/* the maximum index of the i_block array should be computed from i_blocks / ((1024<<s_log_block_size)/512)
		 * or once simplified, i_blocks/(2<<s_log_block_size)
		 * https://www.nongnu.org/ext2-doc/ext2.html#i-blocks
		 */
		unsigned int i_blocks = inode->i_blocks/(2<<super.s_log_block_size);

		printf("number of blocks %u\n", i_blocks);
		
		// S_ISDIR, S_ISREG checks if the file is a directory or a regular file. For this project we want to check if it is a regular file. 
		// After checking if it's a regular file we need to check if it is a jpg file. (Oz)
		printf("Is directory? %s \n Is Regular file? %s\n", S_ISDIR(inode->i_mode) ? "true" : "false", S_ISREG(inode->i_mode) ? "true" : "false");
		
	        int is_jpg = 0;	
		// If inside then it is a regular file
		if (S_ISREG(inode->i_mode)) {
		    
                    // Check if file is a jpg file
		    // Put all inodes that represent regular files into char buffer
		    char buffer[1024];
		    
		    printf("Inode->block[%d] = %d\n", i, inode->i_block[0]);

		    // USE LSEEK AND READ. After finding the BLOCK_OFFSET use Lseek to take you there. Then once there use read.
                    lseek(fd, BLOCK_OFFSET(inode->i_block[0]), SEEK_SET);
                    read(fd, buffer, block_size);
		    // printf("Next block address: %d, char at this address: %c\n", inode->i_block[0], buffer[0]);
		      
		    // check the contents of buffer and check if jpg
    		    if (buffer[0] == (char)0xff &&
				    buffer[1] == (char)0xd8 &&
				    buffer[2] == (char)0xff &&
				    (buffer[3] == (char)0xe0 ||
				     buffer[3] == (char)0xe1 ||
				     buffer[3] == (char)0xe8)) {
			    is_jpg = 1;
		    }

		} // S_ISREG(inode->i_mode)

               // if it is jpeg read all data blocks until filesize is reached. Use i_block pointers;
               if (is_jpg == 1) {
                  
		  // create new file with correct filename
		  int newFD;
		  char filename[MAX_STRING_SIZE];
		  char pathName[MAX_STRING_SIZE];
		  // add to directory
		  sprintf(filename,"file-%d.jpg",i);
		  
		  snprintf(pathName,1024,"%s/%s",dir, filename);
		  newFD = open(pathName, O_RDWR | O_CREAT | O_APPEND, 0666);
		  if (newFD == -1) {
		     printf("Error newFD couldn't be opeded");
		  }
		  printf("Found a JPG file; recovering it to %s\n",filename);
		  // read all data blocks. 
		  int size = (int) inode->i_size;    // inode-> i_size is the size of the regular file
		  
		   int interBuffer[256];
		   int secBuffer[256];
	           for (unsigned int i=0; i<EXT2_N_BLOCKS; i++) {
		       if (i < EXT2_NDIR_BLOCKS) {    				   /* direct blocks */
		           
                            int currentSize = 1024;
                     	    if (size == 0) {
                                break;
                            } else if (size < 1024) {   // update size accordingly
                                currentSize = size;
			    }
                  
                     	    size -= currentSize;           // subtract
                     	    char dataBuffer[block_size];   // = malloc(currentSize);
                     	    lseek(fd, BLOCK_OFFSET(inode->i_block[i]), SEEK_SET);
                     	    read(fd, dataBuffer,currentSize);

                     	    int len = write(newFD, dataBuffer, currentSize);
                     	    printf("The length of file: %d and the length of write: %d\n", size, len);
                     	    printf("Found a JPG file; recovering it to %s\n",filename);
		            printf("Block %2u : %u\n", i, inode->i_block[i]); 
		       } else if (i == EXT2_IND_BLOCK) {				  /* single indirect block */
			    lseek(fd, BLOCK_OFFSET(inode->i_block[i]), SEEK_SET);
		            read(fd, interBuffer, block_size);
			    for (int j = 0; j < 256; j++) {
                               int currentSize = 1024;
			       if (size == 0) {
			           break;
				} else if (size < 1024) {   // update size accordingly
			              currentSize = size;
				}
				size -= currentSize;           // subtract
				char dataBuffer[block_size];   // = malloc(currentSize);
				// lseek(fd, BLOCK_OFFSET(inode->i_block[i]), SEEK_SET);
                                lseek(fd, BLOCK_OFFSET(interBuffer[j]), SEEK_SET);
				read(fd, dataBuffer,currentSize);
				int len =  write(newFD, dataBuffer, currentSize);
				printf("The length of file: %d and the length of write: %d\n", size, len);
			   }
			   printf("Single   : %u\n", inode->i_block[i]);
		       } else if (i == EXT2_DIND_BLOCK) {                            /* double indirect block */
		              lseek(fd, BLOCK_OFFSET(inode->i_block[i]), SEEK_SET);
                              read(fd, secBuffer, block_size); 
			       for (int k = 0; k < 256; k++) {
			           lseek(fd, BLOCK_OFFSET(secBuffer[k]), SEEK_SET);
                                   read(fd, interBuffer, block_size);
			           for (int j = 0; j < 256; j++) {
			               int currentSize = 1024;
				       if (size == 0) {
				           break;
				       } else if (size < 1024) {   // update size accordingly
				           currentSize = size;
				       }
				       size -= currentSize;           // subtract
				       char dataBuffer[block_size];   // = malloc(currentSize);
				       lseek(fd, BLOCK_OFFSET(interBuffer[j]), SEEK_SET);
				       read(fd, dataBuffer,currentSize);
				       int len =  write(newFD, dataBuffer, currentSize);
				       printf("The length of file: %d and the length of write: %d\n", size, len);
				       }
			       printf("Double   : %u\n", inode->i_block[i]);
			       }
		       } else if (i == EXT2_TIND_BLOCK)                            /* triple indirect block */ // Won't be tested on triple indirect.
			       printf("Triple   : %u\n", inode->i_block[i]);
		   }
		   close(newFD);
	       }  // if (isjpg == True)
		
		free(inode);

	   }    // for (unsigned int i = 0; i < itable; i++)

	}       // for (group = 0; group < groupCount; group++)
	close(fd);
}
