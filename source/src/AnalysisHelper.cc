/// \file AnalysisHelper.cc
/*
 *
 * AnalysisHelper.cc source template automatically generated by a class generator
 * Creation date : mar. juil. 1 2014
 *
 * This file is part of DQM4HEP libraries.
 *
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Tom Coates
 * @copyright CNRS , IPNL
 */

// -- dqm4hep headers
#include <dqm4hep/AnalysisHelper.h>

namespace dqm4hep {

  namespace core {

    float AnalysisHelper::mean(MonitorElementPtr pMonitorElement, float percentage = 1.0)
    {
      TH1 *h = pMonitorElement->objectTo<TH1>();
      float result;

      if (percentage == 1.0) {
	result = h->GetMean();
      }
      else {
	TAxis *axis = h->GetXaxis();
	int nbins = axis->GetNbins();
	int imean = axis->FindBin(h->GetMean());

	float entries = percentage*h->GetEntries();
	float w = h->GetBinContent(imean);
	float x = h->GetBinCenter(imean);
	float sumw = w;
	float sumwx = w*x;

	for (int i=1;i<nbins;i++)
	  {
	    if (i>0)
	      {
		w = h->GetBinContent(imean-i);
		x = h->GetBinCenter(imean-i);
		sumw += w;
		sumwx += w*x;
	      }
	    if (i<= nbins) {
	      w = h->GetBinContent(imean+i);
	      x = h->GetBinCenter(imean+i);
	      sumw += w;
	      sumwx += w*x;
	    }
	    if (sumw > entries) break;
	  }
	      
	result = sumwx/sumw;
      }
      return result;
    }

    float AnalysisHelper::mean90(MonitorElementPtr pMonitorElement)
    {
      float result = AnalysisHelper::mean(pMonitorElement, 0.9);
      return result;
    }

    float AnalysisHelper::rms(MonitorElementPtr pMonitorElement, float percentage = 1.0)
    {
      TH1 *h = pMonitorElement->objectTo<TH1>();
      float result;

      if (percentage == 1.0) {
	result = h->GetRMS();
      }
      else {
	// PLACEHOLDER
	float result = 0.0;
      }
      return result;
    }

    float AnalysisHelper::rms90(MonitorElementPtr pMonitorElement)
    {
      float result = AnalysisHelper::rms(pMonitorElement, 0.9);
      return result;
    }

    float AnalysisHelper::median(MonitorElementPtr pMonitorElement)
    {

      TH1 *h = pMonitorElement->objectTo<TH1>();

      Double_t xq[1];
      Double_t yq[1];
      xq[0] = 0.5;

      h->GetQuantiles(1, yq, xq);
      float result = yq[0];

      return result;
      
    }

  }

}
