package com.pxy.cloudlarkxrpico.Adapter;

import android.content.Context;
import android.database.DataSetObserver;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SpinnerAdapter;
import android.widget.TextView;

import com.pxy.cloudlarkxrpico.Bean.ServerBean;
import com.pxy.cloudlarkxrpico.R;

import java.util.List;

public class ServerListApinnerAdapter implements SpinnerAdapter {
    private Context context;
    private List<ServerBean> serverBeanList;
    public ServerListApinnerAdapter(Context context, List<ServerBean> serverBeanList){
        this.context=context;
        this.serverBeanList=serverBeanList;
    }

    @Override
    public View getDropDownView(int position, View convertView, ViewGroup parent) {
        if (convertView == null) {
            convertView = LayoutInflater.from(context).inflate(R.layout.spinner_item, null);
            TextView textView = convertView.findViewById(R.id.text);
            textView.setText("http://" + serverBeanList.get(position).getIp() + ":" + serverBeanList.get(position).getPort());
        }
        return convertView;
    }

    @Override
    public void registerDataSetObserver(DataSetObserver observer) {

    }

    @Override
    public void unregisterDataSetObserver(DataSetObserver observer) {

    }

    @Override
    public int getCount() {
        return serverBeanList.size();
    }

    @Override
    public Object getItem(int position) {
        return serverBeanList.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public boolean hasStableIds() {
        return false;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        if (convertView == null) {
            convertView = LayoutInflater.from(context).inflate(R.layout.spinner_item, null);
            TextView textView = convertView.findViewById(R.id.text);
            textView.setText("http://" + serverBeanList.get(position).getIp() + ":" + serverBeanList.get(position).getPort());
        }
        return convertView;
    }

    @Override
    public int getItemViewType(int position) {
        return 1;
    }

    @Override
    public int getViewTypeCount() {
        return 1;
    }

    @Override
    public boolean isEmpty() {
        return false;
    }
}
