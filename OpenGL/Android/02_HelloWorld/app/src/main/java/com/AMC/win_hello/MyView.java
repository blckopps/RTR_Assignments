package com.AMC.win_hello;

import androidx.appcompat.widget.AppCompatTextView;
import android.content.Context;
import android.view.Gravity;
import android.graphics.Color;

public class MyView extends AppCompatTextView
{
	public MyView(Context drawingContext)
	{
		super(drawingContext);
		setTextColor(Color.rgb(0, 255, 0));
		setTextSize(60);
		setGravity(Gravity.CENTER);
		setText("HELLO WORLD!!!");
	}
}