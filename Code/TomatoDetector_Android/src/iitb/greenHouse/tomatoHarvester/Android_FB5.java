/**
 * Project Name: Tomato Harvester
 * Author: 	Amandeep Chugh
 * 			Ashish Kumar Agarwal
 * 			Jayasree Kaveti
 * 			Lorin Ahmed
 */
/********************************************************************************

   Copyright (c) 2010, ERTS Lab IIT Bombay erts@cse.iitb.ac.in               -*- c -*-
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   * Source code can be used for academic purpose. 
	 For commercial use permission form the author needs to be taken.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. 

  Software released under Creative Commence cc by-nc-sa licence.
  For legal information refer to: 
  http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode

********************************************************************************/

package iitb.greenHouse.tomatoHarvester;

import iitb.greenHouse.tomatoHarvester.scanner.Sample1View;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;

import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Toast;

/** Main activity which starts when the application is first Run.
 * Task: (1)Initialise the User Interface.
 * 		 (2)Initialise button click listeners.
 * 		 (3)Implements all button click listeners.
 * 		 (4)Call subroutines to connect to Bluetooth  'Connect' button click.
 * 		 (5)Call subroutines to disconnect from bluetooth.
 */
public class Android_FB5 extends Activity {

	final String tag = "Android_FB5";

	/** Bluetooth related objects. */
	
	private BluetoothComm mBluetoothComm = null;
	private static final int REQUEST_ENABLE_BT = 2;
	private BluetoothAdapter mAdapter = null;
	
	/** UI related objects. */
	private Button mSendButton;
	private Button mConnectButton;
	private Button mDisconnectButton;
	private ImageView mImageView;
	
	
	private Button mForwardButton;
	private Button mLeftButton;
	private Button mRightButton;
	private Button mBackButton;
	private Button mStopButton;
	
	private Button mPulleyUpButton;
	private Button mPulleyDownButton;
	private Button mPulleyStopButton;
	private Button mCutterOnButton;
	private Button mCutterOffButton;
	private Button mCameraOnButton;
	private Button mBotSpeedSendButton;
	private EditText mBotSpeedTextBox;
	
	private Button mSetTrough;
	private EditText mSetTroughTextBox;
	
	
	
	private Sample1View         mView;
	 private BaseLoaderCallback  mOpenCVCallBack = new BaseLoaderCallback(this) {
	    	@Override
	    	public void onManagerConnected(int status) {
	    		switch (status) {
					case LoaderCallbackInterface.SUCCESS:
					{
						Log.i(tag, "OpenCV loaded successfully");
						
						//give control back to firebird
						
						byte[] write_buffer = new byte[4];
						write_buffer[0] = (byte)'M';
						write_buffer[1] = (byte)'1';
						write_buffer[2] = 0x0d;
						write_buffer[3] = 0x0a;
						

						
						
						try {mBluetoothComm.BluetoothSend(write_buffer);   
						}
						catch (Exception e){e.printStackTrace();
						}
						Log.d(tag, "Write on button press successful:: " + write_buffer);   
						
						
						// Create and set View
						mView = new Sample1View(mAppContext, mBluetoothComm);
						setContentView(mView);
						// Check native OpenCV camera
						if( !mView.openCamera() ) {
							AlertDialog ad = new AlertDialog.Builder(mAppContext).create();
							ad.setCancelable(false); // This blocks the 'BACK' button
							ad.setMessage("Fatal error: can't open camera!");
							ad.setButton("OK", new DialogInterface.OnClickListener() {
							    public void onClick(DialogInterface dialog, int which) {
								dialog.dismiss();
								finish();
							    }
							});
							ad.show();
						}
					} break;
					default:
					{
						super.onManagerConnected(status);
					} break;
				}
	    	}
		};
	
	
	
	
	/** Called when the activity is first created. 
	 *  Starts all the button click listeners.*/
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.main); /** Set the layout of UI from "/res/layout/main". */
		
		Log.d(tag,"Android FB5 started..");

		mAdapter = BluetoothAdapter.getDefaultAdapter(); /** Get the Bluetooth hardware and create a handle for it. */
		if (mAdapter == null) {
			Toast.makeText(this, "Bluetooth is not available. Closing Application", Toast.LENGTH_LONG).show();
			finish();
			return;
		}

		mSendButton = (Button) findViewById(R.id.button_send); /** Start listeners for button click.*/
		mSendButton.setOnClickListener(SendListener);
		
		mConnectButton = (Button)this.findViewById(R.id.connect);
		mConnectButton.setOnClickListener(ConnectListener);      
		
		mDisconnectButton = (Button)this.findViewById(R.id.disconnect);
		mDisconnectButton.setOnClickListener(DisconnectListener);      
		
		mDisconnectButton.setEnabled(false); /** Enable only 'Connect' button initially. */
		mConnectButton.setEnabled(true);
		mSendButton.setEnabled(false);
		
		mImageView = (ImageView) findViewById(R.id.ImageView);
		mImageView.setImageBitmap(null);
		
		
		/*Various buttons for manual control of the bot*/
		
		mForwardButton = (Button)this.findViewById(R.id.button_forward);
		mForwardButton.setOnClickListener(forwardListener);     
		
		mLeftButton = (Button)this.findViewById(R.id.button_left);
		mLeftButton.setOnClickListener(leftListener);     
		
		mRightButton = (Button)this.findViewById(R.id.button_right);
		mRightButton.setOnClickListener(rightListener);     
		
		mBackButton = (Button)this.findViewById(R.id.button_backward);
		mBackButton.setOnClickListener(backListener);     
		
		mStopButton = (Button)this.findViewById(R.id.button_stop);
		mStopButton.setOnClickListener(stopListener);     
		
		mPulleyUpButton = (Button)this.findViewById(R.id.button_pulleyUp);
		mPulleyUpButton.setOnClickListener(pulleyUpListener);     
		
		mPulleyDownButton = (Button)this.findViewById(R.id.button_pulleyDown);
		mPulleyDownButton.setOnClickListener(pulleyDownListener);     
		
		mPulleyStopButton = (Button)this.findViewById(R.id.button_pulleyStop);
		mPulleyStopButton.setOnClickListener(pulleyStopListener);     
		
		mCutterOnButton = (Button)this.findViewById(R.id.button_CutterOn);
		mCutterOnButton.setOnClickListener(cutterOnListener);     
		
		mCutterOffButton = (Button)this.findViewById(R.id.button_CutterOff);
		mCutterOffButton.setOnClickListener(cutterOffListener);   
		
		mCameraOnButton = (Button)this.findViewById(R.id.button_CameraOn);
		mCameraOnButton.setOnClickListener(cameraOnListener); 
		
		mBotSpeedSendButton = (Button)this.findViewById(R.id.btnSndSpeed);
		mBotSpeedSendButton.setOnClickListener(SpeedSendListener);  
		mBotSpeedTextBox = (EditText) findViewById(R.id.etBotSpeed);
		
		mSetTrough = (Button)this.findViewById(R.id.btnSetTrough);
		mSetTrough.setOnClickListener(SetTroughListener);  
		mSetTroughTextBox = (EditText) findViewById(R.id.etTroughNumber);
		
		
	}


	/** Called when 'Connect' button is clicked. Starts the connection procedure over BT*/
	private OnClickListener ConnectListener = new OnClickListener()  
	{  
		public void onClick(View v)  
		{         
			Log.d(tag,"Connect Requested");    
			startup();  /** Start the BT connection process.*/ 
			
		}  
	};

	/** Called when 'Disconnect' button is pressed. Frees the BT channel. */
	private OnClickListener DisconnectListener = new OnClickListener()  
	{  
		public void onClick(View v)  
		{         
			Log.d(tag,"Disonnect Requested");    
			if(mBluetoothComm != null)mBluetoothComm.free_channel(); /**Free up the BT channel. */
			mConnectButton.setEnabled(true);
			mDisconnectButton.setEnabled(false);
			mSendButton.setEnabled(false);
		}    
	};

	/** Only used for testing purpose. */
	private OnClickListener SendListener = new OnClickListener() {
		public void onClick(View v) {

			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = 'F';
			write_buffer[1] = '5';
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + write_buffer);   

		}
	};	
	
	/** Called when 'Forward' button is pressed */
	private OnClickListener forwardListener = new OnClickListener() {
		public void onClick(View v) {

			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = 'F';
			write_buffer[1] = 'F';
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + write_buffer);  

		}
	};	
	
	/** Called when 'Left' button is pressed */
	private OnClickListener leftListener = new OnClickListener() {
		public void onClick(View v) {

			
			char bot_speed = mBotSpeedTextBox.getText().toString().charAt(0);
			
			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = 'L';
			write_buffer[1] = (byte) bot_speed;
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + write_buffer);   

		}
	};
	
	/** Called when 'Right' button is pressed */
	private OnClickListener rightListener = new OnClickListener() {
		public void onClick(View v) {

			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = 'R';
			write_buffer[1] = 'R';
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + write_buffer);   

		}
	};	
	
	/** Called when 'Back' button is pressed */
	private OnClickListener backListener = new OnClickListener() {
		public void onClick(View v) {

			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = 'B';
			write_buffer[1] = 'B';
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + write_buffer);   

		}
	};	
	
	/** Called when 'Stop' button is pressed */
	private OnClickListener stopListener = new OnClickListener() {
		public void onClick(View v) {

			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = 'S';
			write_buffer[1] = 'S';
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + write_buffer);  

		}
	};	
	
	/** Called when 'Pulley Up' button is pressed */
	private OnClickListener pulleyUpListener = new OnClickListener() {
		public void onClick(View v) {

			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = 'U';
			write_buffer[1] = 'U';
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + write_buffer);   

		}
	};	
	
	/** Called when 'Pulley Down' button is pressed */
	private OnClickListener pulleyDownListener = new OnClickListener() {
		public void onClick(View v) {

			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = 'K';
			write_buffer[1] = 'K';
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + (char)write_buffer[0]  + " " + (char)write_buffer[0]);   

		}
	};	
	
	
	/** Called when 'Pulley Stop' button is pressed */
	private OnClickListener pulleyStopListener = new OnClickListener() {
		public void onClick(View v) {

			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = 'G';
			write_buffer[1] = 'G';
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + write_buffer);   

		}
	};	
	
	/** Called when 'Cutter On' button is pressed */
	private OnClickListener cutterOnListener = new OnClickListener() {
		public void onClick(View v) {

			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = 'Z';
			write_buffer[1] = 'Z';
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + write_buffer);   

		}
	};	
	
	/** Called when 'Cutter Off' button is pressed */
	private OnClickListener cutterOffListener = new OnClickListener() {
		public void onClick(View v) {

			
			char bot_speed = mBotSpeedTextBox.getText().toString().charAt(0);
			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = 'E';
//			write_buffer[1] = (byte) bot_speed;
			write_buffer[1] = 'E';
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + write_buffer);   

		}
	};	
	
	/** Called when 'Camera On' button is pressed */
	private OnClickListener cameraOnListener = new OnClickListener() {
		public void onClick(View v) {

			 Log.i(tag, "Trying to load OpenCV library");
		        if (!OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_2, Android_FB5.this, mOpenCVCallBack))
		        {
		        	Log.e(tag, "Cannot connect to OpenCV Manager");
		        }
			
			
			Log.d(tag, "abc" );   

		}
	};	
	
	/** Called when 'Speed Send' button is pressed */
	private OnClickListener SpeedSendListener = new OnClickListener() {
		public void onClick(View v) {
			
			char bot_speed = mBotSpeedTextBox.getText().toString().charAt(0);
			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = (byte)'Y';
			write_buffer[1] = (byte) bot_speed;
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + write_buffer);  

		}
	};	
	
	/** Called when 'Set Trough' button is pressed */
	private OnClickListener SetTroughListener = new OnClickListener() {
		public void onClick(View v) {
			
			char troughNo = mSetTroughTextBox.getText().toString().charAt(0);
			
			byte[] write_buffer = new byte[4];
			write_buffer[0] = (byte)'T';
			write_buffer[1] = (byte) troughNo;
			write_buffer[2] = 0x0d;
			write_buffer[3] = 0x0a;
			

			try {mBluetoothComm.BluetoothSend(write_buffer);   
			}
			catch (Exception e){e.printStackTrace();
			}
			Log.d(tag, "Write on button press successful:: " + write_buffer);  

		}
	};	
	
	/** Called when the activity starts. Gives a request to turn ON the Bluetooth id OFF*/
	@Override
	public void onStart() {
		super.onStart();
		Log.d(tag, "++ ON START ++");
		/** If bluetooth is not enabled, ask for user permission to turn on bluetooth. */
		if (!mAdapter.isEnabled()) {
			Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
			startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
		} 
	}



	/** Called when the activity resumes. */
	@Override
	public synchronized void onResume() {
		super.onResume();
		
	}
	

	/** Called when the activity is aborted. 
	 * Stops the BT channel. */
	@Override
	public void onDestroy() {
		super.onDestroy();
	
		if(mBluetoothComm != null){
			mBluetoothComm.free_channel();}
		Log.e(tag, "--- ON DESTROY ---");
	}

	/** Initialisation function.
	 * Called from : 'Connect' button click listener.
	 * Task: Establish connection between phone and bluetooth module on FB5. 
	 * Arguments : Null 
	 * Return : Null
	 */
	private void startup()
	{
		mBluetoothComm = new BluetoothComm(this);
		Toast.makeText(this, "Connecting...", Toast.LENGTH_LONG).show();
		try {
			Log.d(tag, "Initialisation Started...");
			
			/** Bluetooth initialise function returns true if connection is succesful, else false. */
			if(mBluetoothComm.Initialise() == false) 
			{
				Toast.makeText(this, " No connection established ", Toast.LENGTH_SHORT).show();
				return;
			}
			else 
			{
				Toast.makeText(this, " Connection established ", Toast.LENGTH_SHORT).show();
			}
			Log.d(tag, "Initialisation Successful");
		} catch (Exception e) {
			e.printStackTrace();
			Log.e(tag, "Initialisation Failed");
		}
		
		/** Enable 'Disconnect' button. */
		mDisconnectButton.setEnabled(true);
		mConnectButton.setEnabled(false);
		mSendButton.setEnabled(true);
	}

	/** Called when the activity resumes after prompting user to turn ON the bluetooth. 
	 * If turned ON, goes ahead with application, else closes the connection and stops application.
	 */
	public void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		Log.d(tag, "onActivityResult " + resultCode);
		if (requestCode == REQUEST_ENABLE_BT) 
		{
			/** When the request to enable Bluetooth returns. */
			if (resultCode == Activity.RESULT_OK) {
				Log.d(tag,"BT Enabled");
				// Bluetooth is now enabled
			} else {
				// User did not enable Bluetooth or an error occured
				Log.d(tag, "BT not enabled");
				Toast.makeText(this, "Bluetooth was not enabled. Closing application..", Toast.LENGTH_LONG).show();
				finish();  /** Terminate the activity and close application. */
				return;
			}
		}
	}

}