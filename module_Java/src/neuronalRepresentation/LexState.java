package neuronalRepresentation;

import java.util.LinkedList;
import java.io.DataOutputStream;
import java.io.IOException;

public class LexState {
	private int N;
	private LinkedList<Integer> xindex;
	private LinkedList<Integer> yindex;
	private int xactive;
	private int yactive;
	
	public LinkedList<Integer> getXindex() {
		return xindex;
	}

	public LexState(int number_neurons){
		this.N=number_neurons;
		xindex = new LinkedList<Integer>();
		yindex = new LinkedList<Integer>();
	}
	
	public LexState(int number_neurons, LinkedList<Integer> xindex, LinkedList<Integer> yindex){
		this.N=number_neurons;
		setX(xindex);
		setY(yindex);
	}

	public boolean[] getX() {
		boolean[] ret = new boolean[N];
		for (int i=0;i<xactive;i++){
			ret[xindex.get(i)]=true;
		}
		return ret;
	}

	public void setX(LinkedList<Integer> xindex) {
		this.xindex=xindex;
		if (xindex!=null) xactive = xindex.size();
	}

	public boolean[] getY() {
		boolean[] ret = new boolean[N];
		for (int i=0;i<yactive;i++){
			ret[yindex.get(i)]=true;
		}
		return ret;
	}

	public void setY(LinkedList<Integer> yindex) {
		this.yindex=yindex;
		if (yindex!=null) yactive = yindex.size();
	}
    
    public int getNumberNeurons(){
        return this.N;
    }
    
    public void writefile(DataOutputStream os){
    	try {
    		//System.out.println("x:"+xactive+" - y:"+yactive);
			os.writeInt(xactive);
			for (int i =0;i<xactive;i++){
				os.writeInt(xindex.get(i));
			}
			os.writeInt(yactive);
			for (int i =0;i<yactive;i++){
				os.writeInt(yindex.get(i));
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
    }

}
