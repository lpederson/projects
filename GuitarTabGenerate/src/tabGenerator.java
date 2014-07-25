import java.util.*;

public class tabGenerator 
{
	public static void main(String[] args)
	{
		tabGenerator myTab = new tabGenerator();
		myTab.generateTab(5,3);
		myTab.display();
		myTab.generateTab(0,5);
		myTab.display();
	}
	
	public static int [][] myTab = new int [6][40];
	public int rows = 6;
	public int cols = 39;
	int startingString;
	int startingNote;
	int maxNoteChange = 3;
		
	public void generateTab(int startingNote,int startingString)
	{		
		this.startingNote = startingNote;
		this.startingString = startingString;
		
		int note = startingNote;
		int nextNote = note;
		int string = startingString;
		int nextString = 0;
		
		int skip;
		int upOrDown;
		
		Random generator = new Random();
		
		//Initialize
		for(int i=0;i<rows;i++)
		{
			for(int j=0;j<cols;j++)
			{
				myTab[i][j] = -1;
			}	
		}
		
	
		for(int j=0;j<cols;j++)
		{
			skip = generator.nextInt(100);
			if(skip < 90) // 90% chance won't happen
			{
				//Do work
				myTab[string][j] = note;
				string++;
				if(note == 0 && string == 6)
				{
					nextNote = generator.nextInt(100);
					if(nextNote < 80)
					{
						string--;
						continue;
					}
				}
				
				nextNote = generator.nextInt(maxNoteChange)+1;
				upOrDown = generator.nextInt(100);
				
				if(upOrDown < 50) // 50% chance
				{
					upOrDown *= -1;
				}
				
				if(upOrDown > 0)
				{
					note += nextNote;
				}
				else
				{
					note -= nextNote;
				}
				
				if(note < 0)
				{
					note = 0;
				}
				if(note > 21)
				{
					note = 20;
				}
				
				nextString = generator.nextInt(maxNoteChange);
				upOrDown = generator.nextInt(100);
				
				if(upOrDown < 50)
				{
					upOrDown *= -1;
				}
				if(upOrDown < 0)
				{
					string -=  nextString;
				}
				else
				{
					string += nextString;
				}
				string--;
				if(string < 0)
				{
					string = 0;
				}
				if(string > 5)
				{
					string = 5;
				}
				if(string == 5)
				{
					note--;
					note--;
				}
				//Account for array... 
				//string --;
			}
		}
	}
	public void display() 
	{		
		//Display
		for(int i=0;i<rows;i++)
		{
			for(int j=0;j<cols;j++)
			{
				if(myTab[i][j] < 0)
				{
					System.out.print("---");
				}
				else
				{
					if(myTab[i][j] < 10)
					{
						System.out.print("-");
					}
					System.out.print(myTab[i][j] + "-");
				}
			}
			System.out.println();
		}
		System.out.println();
	}
}
