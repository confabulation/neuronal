package neuronalRepresentation;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.LinkedList;
import java.util.Random;

public class Lexicon {

	private static int N;
	private int L;
	private boolean[][] W;
	private LexState[] LState;

	/**
	 * Constructor
	 * @param number_neurons
	 * @param number_symbols 
	 */
	public Lexicon(int number_neurons, int number_symbols){
		this.N=number_neurons;
		this.L=number_symbols;
		W = new boolean[N][N];
		LState = new LexState[L];
		init_network();
	}

	/**
	 * create all the symbols and the inter-connection matrix W
	 *  
	 */
	public void init_network(){
		Random rand = new Random();
		LinkedList<Integer> xlist;
		LinkedList<Integer> ylist;

		//Creating L pairs of stable states
		for (int i=0;i<L;i++){
			//create new symbol
			LState[i]=new LexState(N);
			//init x and y list that contains the representation of the symbol
			xlist = new LinkedList<Integer>();
			ylist = new LinkedList<Integer>();
			//generate collection of neuron for x (representation in thalamus)
			for (int j=0;j<60;j++){
				int a = rand.nextInt(N);
				if (xlist.contains(a)) {
					j--;
				}
				else{
					xlist.add(a);
				}
			}
			//generate collection of neuron for y (representation in cortex)
			for (int j=0;j<60;j++){
				int b = rand.nextInt(N);
				if (ylist.contains(b)) {
					j--;
				}
				else{
					ylist.add(b);
				}
			}

			//save pair of x,y vector to state list
			LState[i].setX(xlist);
			LState[i].setY(ylist);

			//adding results to connection matrix W (W = sum(y*x') )
			for (int k=0;k<xlist.size();k++){
					for (int l=0;l<ylist.size();l++){
						W[xlist.get(k)][ylist.get(l)]=true;
					}
			}
		}
	}

	public void writeLexState(String filename){
		DataOutputStream os;
		try {
			os = new DataOutputStream(new FileOutputStream(filename));

			for (int i=0;i<L;i++){
				//if (i%250==0) System.out.println(i);
				LState[i].writefile(os);
			}
			os.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}


	/**
	 * 
	 * @return list of lexicon states (pairs of x,y vectors)
	 */
	public LexState[] getLexStateList(){
		return LState;
	}

	/**
	 * 
	 * @return one random x vector from the list of lexicon states
	 */
	public boolean[] getRandomX(){
		Random rand = new Random();
		int rand_index_x = rand.nextInt(L);
		//System.out.println("index chosen for random: "+rand_index_x);
		return LState[rand_index_x].getX();
	}
	
	public LinkedList<Integer> getRandomXIndex(){
		Random rand = new Random();
		int rand_index_x = rand.nextInt(L);
		return LState[rand_index_x].getXindex();
	}

	/**
	 * 
	 * @param init_vector
	 * @param make_active: range in which neurons will be consider active
	 * @return the state after one cycle of execution
	 */
	public boolean[] execute_step_network(boolean[] init_vector, int make_active){
		//before running the network, compute input excitation I using I = Wu
		/*Note : here we can no longer use boolean as we will need to make active the neurons with the most connection
		 * so we will use a function to change the boolean array to an integer array
		 */
		int[] I = computeExcitation(W,init_vector,false);
		//make active the best neurons in I
		boolean[] v = makeActive(I,make_active);
		//do the reverse operation Iinv = W'v
		int[] Iinv = computeExcitation(W,v,true);
		//again activate the best neuron in result
		boolean[] resultIteration = makeActive(Iinv, make_active);
		return resultIteration;
	}

	/**
	 * 
	 * @param vector: the vector to be modified
	 * @param number_modification: the number of element to change on that vector 
	 * @return a new vector where "number_element" element of "vector" has been changed
	 */
	public boolean[] modify_vector_random(boolean[] vector, int number_modification){
		Random rand = new Random();
		boolean[] ret = vector.clone();
		int tempindex = 0;
		for (int i=0;i<number_modification;i++){
			tempindex = rand.nextInt(vector.length);
			if (ret[tempindex]==true) ret[tempindex]=false;
			else ret[tempindex]=true;
		}
		return ret;
	}
	
	/**
	 * remove a given number of 1 from vector and add some random noise
	 * @param vector : the vector to be modified
	 * @param erase : number of 1 to remove from vector
	 * @param noise : number of noise to add
	 * @return a new vector where "erase" element from "vector" have been removed and some "noise" added 
	 */
	public boolean[] modify_vector(boolean[] vector, int erase, int noise){
		Random rand = new Random();
		boolean[] ret = vector.clone();
		int tempindex = 0;
		LinkedList<Integer> vectorindex = new LinkedList<Integer>();
		for (int i=0;i<vector.length;i++){
			if (vector[i]==true) vectorindex.add(i);
		}
		for (int i=0;i<erase;i++){
			tempindex = rand.nextInt(vectorindex.size());
			if (ret[vectorindex.get(tempindex)]==true) ret[vectorindex.get(tempindex)]=false;
			else i--;
		}
		for (int i=0;i<noise;i++){
			tempindex = rand.nextInt(vector.length);
			if (ret[tempindex]==true) i--;
			else ret[tempindex]=true;
		}
		return ret;
	}
	
	/**
	 * remove a certain number of 1 from vector, add some noise and take some part of other symbols to
	 * add it to the new vector.
	 * @param vector : the vector to be modified
	 * @param erase : number of 1 to remove from vector
	 * @param noise : number of noise to add
	 * @param number_other_symbols : number of other symbol to add to the vector
	 * @param neurons_other_symbols : number of 1 from other symbols that are copied to the vector
	 * @return a new vector where "erase" element of it has been removed, and a few neurons from other symbols
	 * have been added as well as some noise.
	 */
	public boolean[] modify_vector_2(boolean[] vector, int erase, int noise, int number_other_symbols, int neurons_other_symbols){
		boolean[] ret = vector.clone();
		ret = modify_vector(vector,erase,noise);
		Random rand = new Random();
		//add other neuron from other symbols
		for (int i=0; i<number_other_symbols;i++){
			LinkedList<Integer> temp = getRandomXIndex();
			
			for (int j=0;j<neurons_other_symbols;j++){
				int tempindex = rand.nextInt(temp.size());
				ret[temp.get(tempindex)]=true;
			}
		}
		
		return ret;
	}

	/**
	 * W and u are boolean matrices, assume true=1 and false=0 and return W*u
	 * @param W : matrix dimensions should be at least 1x1
	 * @param u : matrix dimension should agree with W (or W^T, if transposeW )
	 * @param transposeW : state whether matrix W must be transpose or not
	 * @return W*u or W'*u if transposeW is true
	 */
	public static int[] computeExcitation(boolean[][] W, boolean[] u, boolean transposeW){
		int[] ret = new int[N];
		if (transposeW){
			for (int i=0;i<u.length;i++){
				if (u[i]==true){
				for (int j=0;j<N;j++){
					if (W[j][i]==true)
						ret[j]=ret[j]+1;
				}
				}
			}
		}
		else{
			for (int i=0;i<u.length;i++){
				if (u[i]==true){
				for (int j=0;j<N;j++){
					if (W[i][j]==true)
						ret[j]=ret[j]+1;
				}
				}
			}
		}
		
		return ret;
	}

	/*public static int[] computeExcitation(Number[][] W, boolean[] u, boolean transposeW){
		int outsize;
		int insize;
		if (transposeW){
			insize = W.length;
			if ( W.length > 0 ){
				outsize = W[0].length;
			}
			else {
				throw new RuntimeException("empty matrix multiplication");
			}
		} else {
			outsize = W.length;
			if ( W.length > 0 ){
				insize = W[0].length;
			}
			else {
				throw new RuntimeException("empty matrix multiplication");
			}
		}

		if (insize != u.length){
			throw new RuntimeException("input u must agree with matrix dimension");
		}

		int[] ret = new int[outsize];
		for (int j=0;j<outsize;j++){
			for (int i=0;i<insize;i++){
				if (u[i] == true){
					if (transposeW){
						if (W[i][j].intValue() != 0 )
							ret[j]=ret[j]+(int) W[i][j].intValue();
					}
					else{
						if (W[j][i].intValue() != 0 )
							ret[j]=ret[j]+(int) W[j][i].intValue();
					}
				}
			}
		}
		return ret;
	}*/

	/**
	 * 
	 * @param I
	 * @param range
	 * @return a vector v where the neurons within range of the max excitation of I has been activated
	 */
	
	public static boolean[] makeActive(int[] I, int range){
		boolean[] ret = new boolean[I.length];
		//first get maximum of "input excitation"
		int max = I[0];
		for (int i=0;i<I.length;i++){
			if (I[i]>max) max=I[i];
		}
		//then active only the neurons in the range of that max
		//Note that the second condition && I[i]!=0 is there to prevent to activate neurons who didn't receive any
		//input excitation
		for (int i=0;i<I.length;i++){
			if (max-I[i]<= range && I[i]!=0) ret[i]=true;
		}
		return ret;
	}

	/**
	 * 
	 * @param v1
	 * @param v2
	 * @return the number of difference between the 2 vectors
	 * @throws LexiconException
	 */
	public int compareVector(boolean[] v1, boolean[] v2) throws LexiconException{
		if (v1.length != v2.length) throw new LexiconException("Error, vector dimension must agree");
		int diff = 0;
		for (int i=0;i<v1.length;i++){
			if ((v1[i]==true && v2[i]==false) || (v1[i]==false && v2[i]==true)) diff++;
		}
		return diff;
	}
	
	/**
	 * Compare given vector against all existing symbols to see if it is equal to one.
	 * @param v
	 * @return
	 * @throws LexiconException
	 */
	public boolean comparetoAllVector(boolean[] v) throws LexiconException{
		for (int i=0;i<L;i++){
			if (compareVector(v,LState[i].getX())==0) return true;
		}
		return false;
	}
}
