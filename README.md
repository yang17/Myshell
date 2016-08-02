# Myshell user’s manual

***By YangMing***

## 1.Execute 
Enter the following in terminal:   

   $`gcc -o myshell myshell.c`
    
   $`chmod 777 myshell.c`
   
   $`./myshell`
   

## 2.Interface
![](file:///Users/mingyang/Desktop/%E5%B1%8F%E5%B9%95%E5%BF%AB%E7%85%A7%202016-08-02%20%E4%B8%8A%E5%8D%888.43.52.png)


## 3.Commmand

 + ###External commands
 
 	Build a folder : $ `mkdir folder`
 	
 	Show all the files or folder: $ `ls -l`
 	
	![](file:///Users/mingyang/Desktop/%E5%B1%8F%E5%B9%95%E5%BF%AB%E7%85%A7%202016-08-02%20%E4%B8%8A%E5%8D%8810.53.25.png)
 	
 	> ###Analysis:
 	 In external commmands, first we use system call  **fork()** to ceate a child process,it returns to the parent the process ID number (PID) of its child.
 	 Then use system call **execvp()** to replaces the current running program with an entirely new one.The parent process can continue doing other things, and it can even keep tabs on its children, using the system call **wait()**.
 	
 
 	 
 + ###Internal commands
 
 **1.Change path to :**
 $ `cd`
 
 ![](file:///Users/mingyang/Desktop/%E5%B1%8F%E5%B9%95%E5%BF%AB%E7%85%A7%202016-08-02%20%E4%B8%8A%E5%8D%8810.53.25.png)
 
 	> ###Analysis: 
 	function **chdir()** changes the current working directory to that specified in path
 
 **2.Get help mannual:**
 $ `help`
 
	![](file:///Users/mingyang/Desktop/%E5%B1%8F%E5%B9%95%E5%BF%AB%E7%85%A7%202016-08-02%20%E4%B8%8A%E5%8D%8810.53.58.png)
 
 	> ###Analysis: 
 	Myshell_help() function just go throuth the builtin_function()
 	  	
 	>  	   for (i = 0; i < Mysh_num_builtins(); i++) {
         printf("  %s\n", builtin_str[i]);
 	
 
 **3.See the history of command:**
 $ `history`
 
 ![](file:///Users/mingyang/Desktop/%E5%B1%8F%E5%B9%95%E5%BF%AB%E7%85%A7%202016-08-02%20%E4%B8%8A%E5%8D%8810.54.17.png)
 
 	> ###Analysis:
 	> Save the everytime's command in an arry hist[], use a pointer 'current' to account how many command have been put in to hist[]. The history function just simplely go through the hist[]
 	>   
 		int Mysh_history(char **args,char *hist[], int current)
		{
        int i = current;
        int hist_num = 1;
        do {
                if (hist[i]) {
                        printf("%4d  %s\n", hist_num, hist[i]);
                        hist_num++;
                }
                i = (i + 1) % HISTORY_COUNT;
        } while (i != current);
        return 1;
		}
  
 **4.Clear the history:**
 $ `hc`
 
 
 ![](file:///Users/mingyang/Desktop/%E5%B1%8F%E5%B9%95%E5%BF%AB%E7%85%A7%202016-08-02%20%E4%B8%8A%E5%8D%8810.54.27.png)
 	> ###Analysis:
 	> go through the hist[], set it free, set it NULL.
 	
   
 **5.Execute the last commmand:**
 $ `!!`
 ![](file:///Users/mingyang/Desktop/%E5%B1%8F%E5%B9%95%E5%BF%AB%E7%85%A7%202016-08-02%20%E4%B8%8A%E5%8D%8810.55.00.png)
 	> ###Analysis:
 	> First clear the last command in hist[],which is "!!"
 	>
 	> Then execute the hist[current -2 ] command
 	>		 
 	>		 free(hist[current-1]);
 			hist[current-1] = NULL;
  			char *newline;
  			newline=strdup(hist[current-2]);
  			//  printf("newline:%s\n", newline);
 			args = Mysh_split_line(newline);
 			return Mysh_execute(args,hist,current);
    
 **6.Execute the nth commmand:**
 	$ `!n`
 
 
 ![](file:///Users/mingyang/Desktop/%E5%B1%8F%E5%B9%95%E5%BF%AB%E7%85%A7%202016-08-02%20%E4%B8%8A%E5%8D%8810.56.37.png)
 > ###Analysis: 
 Since we should transform the **char n** into **int n** by using function **atoi()** ,I do not write this in buildin_function, though it is a build in funcition. Rather , I write it in execute function after it had already decide when the **arg** is a **!!**. Then I use the function **strtok()** to splite the character **!** ,which reamins **n**,and transform the **char n** into **int n**.
 >
 > 		char *str;
  		str=args[0];
  		char *p;
  		int n;
  		p = strtok(str, "!");
  		n = atoi(p);
  		if ( n > 0 && n < 20) {
    	free(hist[current-1]);
    	hist[current-1] = NULL;
    	char *newline2;
    	newline2=strdup(hist[n-1]);
    	printf("command:%s\n",newline2);
    	args = Mysh_split_line(newline2);
    	return Mysh_execute(args,hist,current);}
 	
 	
 	
 	
 	
## 4.Exit
 
To exit program :$ `exit`
 
 The main process will not call fork(2) and instead just exit the program
successfully.
 
 
## 5 Framework
 
  ![](file:///Users/mingyang/Desktop/os.png)
 
> ###Analysis:
> + **Read line:**
> 
> Using the **getline()** function to get the char array from command line
>
 	getline(&line, &bufsize, stdin)
 	 			
> + **Split line:**
>
>First allocate with a block, and if they do exceed it, reallocate with more space
>
>Using the **strtok()** function to return pointers to within the string we give it, and place **\0**  bytes at the end of each token.
>				
>		c = getchar();
>		if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);}
>
  
> + **Execute:**
> 
> Compare **arg[0]** with builtin function's name, if match ,then enter builtin function
> 
> If not match the builtin function,jude whether it is a !n command.
> 
> Or if not, it is a external commmand, then step into the Mysh_lanuch to fork child process.
> 
>			for (i = 0; i < Mysh_num_builtins(); i++) {
  				if (strcmp(args[0], builtin_str[i]) == 0) {
   					return (*builtin_func[i])(args);
   						}
    				}
    	if ( n > 0 && n < 20) {...}
		return Mysh_launch(args);
		
		


  
 
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    