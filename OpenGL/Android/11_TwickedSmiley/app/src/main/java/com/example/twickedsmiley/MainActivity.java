package com.example.twickedsmiley;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
//manual packages
import android.view.Window;
import android.view.WindowManager;

import android.content.pm.ActivityInfo;
import android.graphics.Color;

public class MainActivity extends AppCompatActivity {

	private GLESView glesView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_main);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.getSupportActionBar().hide();
		
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
								  WindowManager.LayoutParams.FLAG_FULLSCREEN);
								  
		this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		
		this.getWindow().getDecorView().setBackgroundColor(Color.BLACK);
		
		glesView = new GLESView(this);
		
		setContentView(glesView);
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
