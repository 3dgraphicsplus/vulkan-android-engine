package com.threedgraphicsplus.vaetest_java;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;

public class GraphicSurface extends SurfaceView implements SurfaceHolder.Callback2 {

    private static final String tag= "GraphicView";


    // Used to load the 'myapplication' library on application startup.
    static {
        System.loadLibrary("vaetestjava");
    }

    native void setSurfaceView(Surface surface);
    native void setShaders(String ver, String frag);
    native void renderVulkan();
    native void setAssetManager(AssetManager mgr);
    native void createTexture(String path);
    native void cleanup();
    native void preRender();

    Context context;

    private AssetManager mgr;

    public GraphicSurface(Context ctx, AttributeSet att){
        super(ctx,att);
        context = ctx;

        SurfaceHolder holder = getHolder();
        holder.addCallback(this);

    }

    private String getString(String path){
        StringBuilder sb = new StringBuilder();
        InputStream is = null;
        try {
            is = context.getAssets().open(path);
            BufferedReader br = new BufferedReader(new InputStreamReader(is, StandardCharsets.UTF_8 ));
            String str;
            while ((str = br.readLine()) != null) {
                sb.append(str);
                sb.append("\n");
            }
            br.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return sb.toString();
    }
    public GraphicSurface(Context _context) {
        super(_context);
        context =_context;
        SurfaceHolder holder = getHolder();
        holder.addCallback(this);

    }

    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(tag, "surfaceCreated");

        setSurfaceView(getHolder().getSurface());

        mgr = getResources().getAssets();
        setAssetManager(mgr);
        //relative path inside assets folder
        String imageFile = "sample_tex.png";
        createTexture(imageFile);

        String ver = getString("shaders/tri.vert");
        String frag = getString("shaders/tri.frag");

        setShaders(ver,frag);

        preRender();
        //
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(tag, "surfaceDestroyed");
        cleanup();
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        Log.d(tag, "surfaceChanged");

        renderVulkan();
    }

    @Override
    public void surfaceRedrawNeeded(@NonNull SurfaceHolder holder) {
        //renderVulkan();
    }
}
