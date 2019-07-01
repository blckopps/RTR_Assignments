package com.AMC.win_hello;
//By default
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
//By manual
import android.view.Window;
import android.view.WindowManager;

import android.content.pm.ActivityInfo;
import android.graphics.Color;


public class MainActivity extends AppCompatActivity {

	private MyView myView;
	
    @Override
    protected void onCreate(Bundle savedInstanceState)
	{
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_main);
		//read titlebar
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.getSupportActionBar().hide();     //hide title bar
		//make full screen
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
								  WindowManager.LayoutParams.FLAG_FULLSCREEN);
								  
		//forced landscape
		this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		
		//set background color
		this.getWindow().getDecorView().setBackgroundColor(Color.BLACK);
		
		//define our own view
		myView = new MyView(this);
		
		setContentView(myView);
    }
	
	@Override
	protected void onPause()
	{
		super.onPause();
	}
	
	@Override
	protected void onResume()
	{
		super.onResume();
	}
}
