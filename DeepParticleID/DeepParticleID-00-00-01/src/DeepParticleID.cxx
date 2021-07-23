#include "DeepParticleID/DeepParticleID.h"
#include "ModelXGBoost.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"
#include "ParticleID/ParticleID.h"

DeepParticleID::DeepParticleID(int method) {
  
  switch(method) {
    case 0: {
      m_model = new ModelXGBoost("./xgboost.model", 5);
      break;
    }
    default: {
      m_model = 0;
    }
  }
}

bool DeepParticleID::calculate(EvtRecTrack* track) {
  this->reset();
  bool ok = this->readData(track);
  if (!ok) return false;
  ok = m_model->predict(m_features, m_prob);
  return ok;
}

float DeepParticleID::prob(int n) {
  if (n<0 || n>m_prob.size()-1) return 0.0;
  return m_prob[n];
}

void DeepParticleID::reset() {
  m_prob.clear();
  for (int i=0;i<57;++i) m_features[i] = 0.0;
  // Chi value for dedx
  m_features[6] = 9999.0;
  m_features[7] = 9999.0;
  m_features[8] = 9999.0;
  m_features[9] = 9999.0;
  m_features[10] = 9999.0;
  // Chi value for tof
  m_features[36] = 9999.0;
  m_features[37] = 9999.0;
  m_features[38] = 9999.0;
  m_features[39] = 9999.0;
  m_features[40] = 9999.0;
  // Chi value for muc
  m_features[50] = 9999.0;
}

bool DeepParticleID::readData(EvtRecTrack* track) {

  if (!track->isMdcTrackValid()) {
    // Deep PID requires successful tracking
    return false;
  }
  RecMdcTrack *mdcTrk = track->mdcTrack();
  // p_mdc
  m_features[0] = mdcTrk->p();
  // pxy_mdc
  m_features[1] = mdcTrk->pxy();
  // theta_mdc
  m_features[2] = mdcTrk->theta();
  // phi_mdc
  m_features[3] = mdcTrk->phi();
  // charge_mdc
  m_features[4] = mdcTrk->charge();

  if (track->isMdcDedxValid()) {
    // response_dedx
    m_features[5] = 1.0;
    RecMdcDedx* dedxTrk = track->mdcDedx();

    ParticleID *pid = ParticleID::instance();
    pid->init();
    pid->setMethod(pid->methodProbability());
    pid->usePidSys(pid->useDedx());
    pid->setRecTrack(track);
    pid->calculate();

    // chie_dedx
    m_features[6] = pid->chi(0);
    // chimu_dedx
    m_features[7] = pid->chi(1);
    // chipi_dedx
    m_features[8] = pid->chi(2);
    // chik_dedx
    m_features[9] = pid->chi(3);
    // chip_dedx
    m_features[10] = pid->chi(4);
    // probPH_dedx
    m_features[11] = dedxTrk->probPH();
    // nGoodHits_dedx
    m_features[12] = dedxTrk->numGoodHits();
    // nHits_dedx
    m_features[13] = dedxTrk->numTotalHits();
  }

  if(track->isTofTrackValid()) {

    ParticleID *pid = ParticleID::instance();
    pid->init();
    pid->setMethod(pid->methodProbability());
    pid->usePidSys(pid->useTofCorr() | pid->useTof1() | pid->useTof2() | pid->useTofE());
    pid->setRecTrack(track);
    pid->calculate();

    // m_chi_e_tof
    m_features[36] = pid->chi(0);
    // m_chi_mu_tof
    m_features[37] = pid->chi(1);
    // m_chi_pi_tof
    m_features[38] = pid->chi(2);
    // m_chi_k_tof
    m_features[39] = pid->chi(3);
    // m_chi_p_tof
    m_features[40] = pid->chi(4);

    SmartRefVector<RecTofTrack> tofTrkCol = track->tofTrack();
    SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();
    for(;iter_tof != tofTrkCol.end(); iter_tof++ ) {
      TofHitStatus *status = new TofHitStatus;
      status->setStatus((*iter_tof)->status());
      if(!(status->is_barrel())){//endcap
        if( !(status->is_counter()) ) continue; // ? 
        if( status->layer()!=1 ) continue;//layer1
        // tof_e
        m_features[14] = (*iter_tof)->tof();
        // path_e
        m_features[15] = (*iter_tof)->path(); // ? 
        // ph_e
        m_features[16] = (*iter_tof)->ph();
        // qual_e
        m_features[17] = 0.0 + (*iter_tof)->quality();
      }
      else {//barrel
        if( !(status->is_cluster()) ) continue; // ? 
        // tof_b
        m_features[18] = (*iter_tof)->tof();
        // path_b
        m_features[19] = (*iter_tof)->path(); // ? 
        // ph_b
        m_features[20] = (*iter_tof)->ph();
        // qual_b
        m_features[21] = 0.0 + (*iter_tof)->quality();
      }
      delete status;
    }
  }  // check tof

  if(track->isEmcShowerValid()) {
    RecEmcShower* emcShower = (track)->emcShower();
    // nhits_emc
    m_features[22]=emcShower->numHits();
    // z_emc
    m_features[23]=emcShower->z();
    // theta_emc
    m_features[24]=emcShower->theta();
    // e_emc
    m_features[25]=emcShower->energy();
    // eseed_emc
    m_features[26]=emcShower->eSeed();
    // e33_emc
    m_features[27]=emcShower->e3x3();
    // e55_emc
    m_features[28]=emcShower->e5x5();
    // secondm_emc
    m_features[32]=emcShower->secondMoment();
    // latm_emc
    m_features[33]=emcShower->latMoment();
    // a20m_emc
    m_features[34]=emcShower->a20Moment();
    // a42m_emc
    m_features[35]=emcShower->a42Moment();
    if (m_features[26] != 0.0 && m_features[27] != 0.0) {
      // e33_seed_emc
      m_features[29] = m_features[27]/m_features[26];
      // e55_seed_emc
      m_features[30] = m_features[28]/m_features[26];
      // e55_33_emc
      m_features[31] = m_features[28]/m_features[27];
    }
  }

  if(track->isMucTrackValid()) {
    RecMucTrack* rpcTrack = track->mucTrack();
    // response_rpc
    m_features[41] = 1.0;
    // status_rpc
    m_features[42]=rpcTrack->status();
    // endPo_rpc
    m_features[43]=rpcTrack->endPart();
    // brLast_rpc
    m_features[44]=rpcTrack->brLastLayer();
    // ecLast_rpc
    m_features[45]=rpcTrack->ecLastLayer();
    // nhits_rpc
    m_features[46]=rpcTrack->numHits();
    // nlayers_rpc
    m_features[47]=rpcTrack->numLayers();
    // maxLayer_rpc
    m_features[48]=rpcTrack->maxHitsInLayer();
    // depth_rpc
    m_features[49]=rpcTrack->depth();
    // chi2_rpc
    m_features[50]=rpcTrack->chi2();
    // y_rpc
    m_features[51]=rpcTrack->yPos();
    // z_rpc
    m_features[52]=rpcTrack->zPos();
    // px_rpc
    m_features[53]=rpcTrack->px();
    // pz_rpc
    m_features[54]=rpcTrack->pz();
    // distance_rpc
    m_features[55]=rpcTrack->distance();
    // deltaPhi_rpc
    m_features[56]=rpcTrack->deltaPhi();
  }
} 
