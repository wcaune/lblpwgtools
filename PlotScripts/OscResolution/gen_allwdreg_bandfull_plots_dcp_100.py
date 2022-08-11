import ROOT
ROOT.gROOT.SetBatch(1)
import ctypes

def filldiff(up,down, diffgraph):
    n = up.GetN()
    i = 0
    xup = ctypes.c_double(-9.9)
    yup = ctypes.c_double(-9.9)
    xlo = ctypes.c_double(-9.9)
    ylo = ctypes.c_double(-9.9)
    while i<n:
        up.GetPoint(i,xup,yup)
        down.GetPoint(n-i-1,xlo,ylo)
        diffgraph.SetPoint(i,xup.value,yup.value)
        diffgraph.SetPoint(n+i,xlo.value,ylo.value)
        i += 1
    return True

if __name__=="__main__":
	ssth23_vals = [0.42, 0.46, 0.50, 0.54, 0.58]
	level_names = ["1sigma", "90p", "3sigma"]
	level_names.reverse()
	legend_names = ["1#sigma", "90%", "3#sigma"]
	legend_names.reverse()
	cols = [ROOT.kRed+4,ROOT.kRed-3, ROOT.kRed-9]
	ops = [1,1,1]
	cols.reverse()
	ops.reverse()
	#graph_pos = ["lowband","midband","upband"]
	for ssth23 in ssth23_vals:
		can = ROOT.TCanvas()
		h = ROOT.TH2D()
		h.SetTitle(";True #delta_{cp};Allowed #delta_{cp}")
		h.GetXaxis().SetLimits(-1.0, 1.0)
		h.GetYaxis().SetLimits(-1., 1.)
		h.Draw()
		leg = ROOT.TLegend(0.1,0.9,0.9,1.0)
		leg.SetNColumns(3)
		for i in range(len(level_names)):
			gf = ROOT.TFile("1dscan_graphs/dcp_100/allwdreg_bandfull_graphs_ssth23=%lf_%s.root" % (ssth23,level_names[i]))
			bands = gf.Get("BandMG").GetListOfGraphs()
			for j in range(bands.GetSize()):
				bands.At(j).SetFillColorAlpha(cols[i],ops[i])
				bands.At(j).SetLineColor(0)
				if j == 0: leg.AddEntry(bands.At(j), legend_names[i])
				bands.At(j).Draw("F SAME")
		leg.Draw()
		can.Print("allwdreg_plots/dcp_100/allwdreg_bandfull_plot_ssth23=%lf.png" % ssth23)
		can.Print("allwdreg_plots/dcp_100/allwdreg_bandfull_plot_ssth23=%lf.pdf" % ssth23)
