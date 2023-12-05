To run the Part B, run main.exe on Windows or main.sh on MacOS
To run the Part B on command line, type gcc -o main main.c then ./main

When running the program, if you enter -1 at any point, the program will end.

You will be prompted for an account number and pin, all account numbers
are stored as the first column in DataBase.txt. If a wrong account
number is added, this portion will loop forever. If a correct account
number was entered but wrong pin, you will get a message back saying
"INCORRECT PIN x tries left" you have three tries before the system
kicks you out.

If the account number and pin you entered were correct, it will prompt you
with three options, enter 1 for balance, 2 for withdraw and -1 to quit. If you
select 1, you will receive the balance attached to your account number. If you
select 2, you will then be prompted to provide an amount to withdraw. If the account
has insuffcient funds, NSF is returned. This process loops forever until a user quits.
